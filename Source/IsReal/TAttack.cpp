// Fill out your copyright notice in the Description page of Project Settings.


#include "TAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter.h"
#include "AIController.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "Animation/AnimMontage.h"

UTAttack::UTAttack()
{
	NodeName = "Attack";
}

EBTNodeResult::Type UTAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyController* currentController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	AEnemy* currentEnemy = Cast<AEnemy>(currentController->GetCharacter());
	UBlackboardComponent* currentBlackboardComp = currentController->GetBlackboardComponent();
	UAnimInstance* currentAnimInstance = currentEnemy->GetMesh()->GetAnimInstance();
	
	//=============Exception Check=============
	if (!currentController)	return EBTNodeResult::Failed;
	if (currentEnemy == nullptr)	return EBTNodeResult::Failed;
	if (!currentBlackboardComp)	return EBTNodeResult::Failed;
	if (!currentAnimInstance) return EBTNodeResult::Failed;
	//=========================================

	//currentEnemy->GetCharacterMovement()->StopMovementImmediately();
	
	//Stop Attack Motion immediately when Enemy die
	if (currentBlackboardComp->GetValueAsName(TEXT("state")) == TEXT("Die")) {
		if(AttackMontage)
			currentEnemy->StopAnimMontage(AttackMontage);
		return EBTNodeResult::Succeeded;
	}
	
	if(!AttackMontage) return EBTNodeResult::Failed;
	currentEnemy->PlayAnimMontage(AttackMontage);

	FOnMontageEnded MontageEndDelegate;
	MontageEndDelegate.BindUObject(this, &UTAttack::OnAttackMontageEnd, &OwnerComp);
	currentAnimInstance->Montage_SetEndDelegate(MontageEndDelegate, AttackMontage);

	return EBTNodeResult::InProgress;
	
	/*
	APlayerCharacter* target = Cast<APlayerCharacter>(BlackboardComp->GetValueAsObject(targetKey.SelectedKeyName));
	if (!target)	return EBTNodeResult::Failed;
	*/
}

void UTAttack::OnAttackMontageEnd(UAnimMontage* PlayedMontage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	//UBlackboardComponent* BlackboardComp = OwnerComp->GetBlackboardComponent();
	//if(!BlackboardComp) FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);


	if (OwnerComp) {
		//BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Chase));
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UTAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AEnemyController* currentController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if (!currentController)	return EBTNodeResult::Failed;
	AEnemy* currentEnemy = Cast<AEnemy>(currentController->GetCharacter());
	if (currentEnemy == nullptr)	return EBTNodeResult::Failed;
	UAnimInstance* currentAnimInstance = currentEnemy->GetMesh()->GetAnimInstance();
	if (!currentAnimInstance)	return EBTNodeResult::Failed;

	currentAnimInstance->Montage_Stop(0.2f, AttackMontage);

	return Super::AbortTask(OwnerComp, NodeMemory);
}