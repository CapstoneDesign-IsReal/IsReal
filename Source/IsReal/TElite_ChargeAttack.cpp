// Fill out your copyright notice in the Description page of Project Settings.

#include "TElite_ChargeAttack.h"
#include "EnemyController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy_Elite.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Niagara/Classes/NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UTElite_ChargeAttack::UTElite_ChargeAttack()
{
	NodeName = TEXT("Elite Charge Attack (Run)");

	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UTElite_ChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if(!PreChargeMontage) return EBTNodeResult::Failed;
	AAIController* CurrentController = OwnerComp.GetAIOwner();
	if (!CurrentController)	return EBTNodeResult::Failed;
	AEnemy_Elite* CurrentEnemy = Cast<AEnemy_Elite>(CurrentController->GetCharacter());
	if (!CurrentEnemy)	return EBTNodeResult::Failed;
	UAnimInstance* CurrentAnim = CurrentEnemy->GetMesh()->GetAnimInstance();
	if(!CurrentAnim) return EBTNodeResult::Failed;
	UCharacterMovementComponent* MovementComp = CurrentEnemy->GetCharacterMovement();
	if (!MovementComp)	return EBTNodeResult::Failed;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)	return EBTNodeResult::Failed;
	UBoxComponent* ChargeHitBox = CurrentEnemy->GetChargeHitBox();
	if (!ChargeHitBox)	return EBTNodeResult::Failed;


	CurrentController->ClearFocus(EAIFocusPriority::Gameplay);

	CachedOriginalSpeed = MovementComp->MaxWalkSpeed;
	MovementComp->MaxWalkSpeed = ChargeSpeed;
	ChargeTimer = 0.0f;
	isReadyToRun = false;
	isChargeEnd = false;
	isPlayerHit = false; 
	
	//HitBox Setting
	ChargeHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ChargeHitBox->OnComponentHit.AddDynamic(this, &UTElite_ChargeAttack::OnChargeHitPlayer);
	CurrentEnemy->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &UTElite_ChargeAttack::OnChargeHitWall);//Delegate Binding
	
	CurrentEnemy->PlayAnimMontage(PreChargeMontage);
	FOnMontageEnded PreChargeEndDelegate;
	PreChargeEndDelegate.BindUObject(this, &UTElite_ChargeAttack::OnPreChargeEnded);
	CurrentAnim->Montage_SetEndDelegate(PreChargeEndDelegate, PreChargeMontage);

	BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Charge));
	return EBTNodeResult::InProgress;
}

void UTElite_ChargeAttack::OnChargeHitWall(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//ignore Self Collision
	if (!OtherComp || !OtherActor || OtherActor == HitComponent->GetOwner()) return;

	if (OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
	{
		isChargeEnd = true;
		//Spawn VFX
		if (HitWallVFX)
		{
			FRotator VFXRotation = FRotationMatrix::MakeFromZ(-Hit.ImpactNormal).Rotator();
			FVector SpawnLoc = Hit.ImpactPoint + (Hit.ImpactNormal * 10.0f);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), 
				HitWallVFX, 
				SpawnLoc, 
				VFXRotation);
		}
	}
}

void UTElite_ChargeAttack::OnChargeHitPlayer(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherComp || !OtherActor || OtherActor == HitComponent->GetOwner()) return;

	if (isPlayerHit) return;

	APlayerCharacter* CurrentPlayer = Cast<APlayerCharacter>(OtherActor);
	if (!CurrentPlayer) return;
	isPlayerHit = true;
	CurrentPlayer->PlayerHit(ChargeDamage);
}

void UTElite_ChargeAttack::OnPreChargeEnded(UAnimMontage* Montage, bool bInterrupted)
{
	isReadyToRun = true;
}

void UTElite_ChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!isReadyToRun)	return;

	AAIController* CurrentController = OwnerComp.GetAIOwner();
	if (!CurrentController) return;
	AEnemy_Elite* CurrentEnemy = Cast<AEnemy_Elite>(CurrentController->GetCharacter());
	if (!CurrentEnemy) return;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)	return;

	FVector ForwardDir = CurrentEnemy->GetActorForwardVector();
	CurrentEnemy->AddMovementInput(ForwardDir, 1.0f);

	ChargeTimer += DeltaSeconds;

	if (isChargeEnd || ChargeTimer > MaxChargeTime)
	{
		EndChargeAttack(OwnerComp, CurrentEnemy, BlackboardComp);
	}
}

EBTNodeResult::Type UTElite_ChargeAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* CurrentController = OwnerComp.GetAIOwner();
	if (!CurrentController) return Super::AbortTask(OwnerComp, NodeMemory);
	AEnemy_Elite* CurrentEnemy = Cast<AEnemy_Elite>(CurrentController->GetCharacter());
	if (!CurrentEnemy)return Super::AbortTask(OwnerComp, NodeMemory);
	UBoxComponent* ChargeHitBox = CurrentEnemy->GetChargeHitBox();
	if (!ChargeHitBox)	return Super::AbortTask(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)	return Super::AbortTask(OwnerComp, NodeMemory);


	ChargeHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChargeHitBox->OnComponentHit.RemoveDynamic(this, &UTElite_ChargeAttack::OnChargeHitPlayer);
	CurrentEnemy->GetCapsuleComponent()->OnComponentHit.RemoveDynamic(this, &UTElite_ChargeAttack::OnChargeHitWall);
	CurrentEnemy->GetCharacterMovement()->MaxWalkSpeed = CachedOriginalSpeed;
	BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Chase));

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UTElite_ChargeAttack::EndChargeAttack(UBehaviorTreeComponent& OwnerComp, AEnemy_Elite* CurrentEnemy, UBlackboardComponent* BlackboardComp)
{
	UBoxComponent* ChargeHitBox = CurrentEnemy->GetChargeHitBox();
	if (!ChargeHitBox)	return;

	ChargeHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChargeHitBox->OnComponentHit.RemoveDynamic(this, &UTElite_ChargeAttack::OnChargeHitPlayer);
	CurrentEnemy->GetCapsuleComponent()->OnComponentHit.RemoveDynamic(this, &UTElite_ChargeAttack::OnChargeHitWall);

	CurrentEnemy->GetCharacterMovement()->MaxWalkSpeed = CachedOriginalSpeed;
	BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Chase));
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}