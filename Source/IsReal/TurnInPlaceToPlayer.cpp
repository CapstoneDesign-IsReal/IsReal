#include "TurnInPlaceToPlayer.h"
#include "EnemyController_Boss.h"
#include "Enemy_Boss.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UTurnInPlaceToPlayer::UTurnInPlaceToPlayer()
{
    NodeName = "Turn To Player";
    bCreateNodeInstance = true;
    
    bNotifyTick = true;
}

EBTNodeResult::Type UTurnInPlaceToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AEnemyController_Boss* CurrentController = Cast<AEnemyController_Boss>(OwnerComp.GetAIOwner());
    CachedEnemy = CurrentController ? Cast<AEnemy_Boss>(CurrentController->GetCharacter()) : nullptr;
    CachedBlackboard = OwnerComp.GetBlackboardComponent();
    CachedAnimInst = CachedEnemy->GetMesh()->GetAnimInstance();
    CachedPlayer = CachedBlackboard ? Cast<APlayerCharacter>(CachedBlackboard->GetValueAsObject(TargetKey.SelectedKeyName)) : nullptr;

    if (!CachedEnemy || !CachedBlackboard || !CachedAnimInst || !CachedPlayer) return EBTNodeResult::Failed;

    FVector BossLoc = CachedEnemy->GetActorLocation();
    FVector TargetLoc = CachedPlayer->GetActorLocation();

    TargetLoc.Z = BossLoc.Z;

    FRotator BossToPlayer = UKismetMathLibrary::FindLookAtRotation(BossLoc, TargetLoc);
    FRotator BossCurrentRot = CachedEnemy->GetActorRotation();

    float DeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(BossToPlayer, BossCurrentRot).Yaw;
    UE_LOG(LogTemp, Warning, TEXT("To Player Rotation : %f"), DeltaYaw)
    if (FMath::Abs(DeltaYaw) <= StopTolerance)  return EBTNodeResult::Succeeded;    //Succeeded when enemy don't need to rotate

    if (DeltaYaw < 0) PlayingMontage = TurnLeftMontage;
    else if (DeltaYaw >= 0 ) PlayingMontage = TurnRightMontage;
    if (!PlayingMontage)    return EBTNodeResult::Failed;

    CachedEnemy->PlayAnimMontage(PlayingMontage);
    
    return EBTNodeResult::InProgress;
}

void UTurnInPlaceToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!CachedEnemy || !CachedBlackboard || !CachedAnimInst || !CachedPlayer) {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    FVector BossLoc = CachedEnemy->GetActorLocation();
    FVector TargetLoc = CachedPlayer->GetActorLocation();

    TargetLoc.Z = BossLoc.Z;

    FRotator BossToPlayer = UKismetMathLibrary::FindLookAtRotation(BossLoc, TargetLoc);
    FRotator BossCurrentRot = CachedEnemy->GetActorRotation();

    float DeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(BossToPlayer, BossCurrentRot).Yaw;
    

    if (FMath::Abs(DeltaYaw) <= StopTolerance || (CachedAnimInst && !CachedAnimInst->Montage_IsPlaying(PlayingMontage))) {
        if (CachedAnimInst && CachedAnimInst->Montage_IsPlaying(PlayingMontage)) {
            CachedAnimInst->Montage_Stop(0.2f, PlayingMontage);
        }

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type UTurnInPlaceToPlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (CachedEnemy && PlayingMontage)
    {
        if (UAnimInstance* AnimInst = CachedEnemy->GetMesh()->GetAnimInstance())
        {
            AnimInst->Montage_Stop(0.2f, PlayingMontage);
        }
    }
    return Super::AbortTask(OwnerComp, NodeMemory);
}