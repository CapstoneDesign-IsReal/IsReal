// Fill out your copyright notice in the Description page of Project Settings.


#include "TElite_Attack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy_Elite.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "AIController.h"

UTElite_Attack::UTElite_Attack()
{
	NodeName = "Normal Attack";

	bCreateNodeInstance = true;
}

EBTNodeResult::Type UTElite_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* CurrentController = OwnerComp.GetAIOwner();
	AEnemy_Elite* CurrentEnemy = CurrentController ? Cast<AEnemy_Elite>(CurrentController->GetPawn()) : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UAnimInstance* CurrentAnim = CurrentEnemy ? Cast<UAnimInstance>(CurrentEnemy->GetMesh()->GetAnimInstance()) : nullptr;
	LeftClawHitBox = (CurrentEnemy && !LeftClawHitBox) ? Cast<UBoxComponent>(CurrentEnemy->GetLeftClawHitBox()) : LeftClawHitBox;
	RightClawHitBox = (CurrentEnemy && !RightClawHitBox) ? Cast<UBoxComponent>(CurrentEnemy->GetRightClawHitBox()) : RightClawHitBox;
	if (!AttackMontage || !BlackboardComp || !CurrentEnemy ||
		!CurrentAnim || !LeftClawHitBox || !RightClawHitBox) return EBTNodeResult::Failed;
	
	isHitPlayer = false;

	LeftClawHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LeftClawHitBox->OnComponentHit.AddUniqueDynamic(this, &UTElite_Attack::OnPlayerHit);
	RightClawHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RightClawHitBox->OnComponentHit.AddUniqueDynamic(this, &UTElite_Attack::OnPlayerHit);

	CurrentEnemy->PlayAnimMontage(AttackMontage);
	FOnMontageEnded AttackMontageEndDelegate;
	AttackMontageEndDelegate.BindUObject(this, &UTElite_Attack::OnAttackMontageEnd, &OwnerComp);
	CurrentAnim->Montage_SetEndDelegate(AttackMontageEndDelegate, AttackMontage);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UTElite_Attack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	isHitPlayer = false;
	if (!LeftClawHitBox || !RightClawHitBox) return Super::AbortTask(OwnerComp, NodeMemory);
	LeftClawHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftClawHitBox->OnComponentHit.RemoveDynamic(this, &UTElite_Attack::OnPlayerHit);
	RightClawHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightClawHitBox->OnComponentHit.RemoveDynamic(this, &UTElite_Attack::OnPlayerHit);

	AAIController* CurrentController = OwnerComp.GetAIOwner();	
	AEnemy_Elite* CurrentEnemy = CurrentController ? Cast<AEnemy_Elite>(CurrentController->GetPawn()) : nullptr;
	UBlackboardComponent* BlackboardComp = CurrentController ? Cast<UBlackboardComponent>(CurrentController->GetBlackboardComponent()) : nullptr;
	UAnimInstance* CurrentAnim = CurrentEnemy ? Cast<UAnimInstance>(CurrentEnemy->GetMesh()->GetAnimInstance()) : nullptr;
	if (!CurrentController || !BlackboardComp || !CurrentEnemy || !CurrentAnim) return Super::AbortTask(OwnerComp, NodeMemory);;
	

	CurrentAnim->Montage_Stop(0.2f, AttackMontage);

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UTElite_Attack::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (!LeftClawHitBox || !RightClawHitBox) return;

	//reset logic variables
	isHitPlayer = false;
	LeftClawHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftClawHitBox->OnComponentHit.RemoveDynamic(this, &UTElite_Attack::OnPlayerHit);
	RightClawHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightClawHitBox->OnComponentHit.RemoveDynamic(this, &UTElite_Attack::OnPlayerHit);

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UTElite_Attack::OnPlayerHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherComp || !OtherActor || OtherActor == HitComponent->GetOwner()) return;
	if (isHitPlayer) return;

	APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(OtherActor);
	if (!HitPlayer) return;
	HitPlayer->PlayerHit(Damage);

	isHitPlayer = true;
}