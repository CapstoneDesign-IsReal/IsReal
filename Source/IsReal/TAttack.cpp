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
	if (!currentController)	return EBTNodeResult::Failed;
	AEnemy* currentEnemy = Cast<AEnemy>(currentController->GetCharacter());
	if (currentEnemy == nullptr)	return EBTNodeResult::Failed;


	//currentEnemy->GetCharacterMovement()->StopMovementImmediately();

	UAnimInstance* currentAnimInstance = currentEnemy->GetMesh()->GetAnimInstance();

	if (currentAnimInstance) {
		if(!AttackMontage) return EBTNodeResult::Failed;
		currentEnemy->PlayAnimMontage(AttackMontage);

		FOnMontageEnded MontageEndDelegate;
		MontageEndDelegate.BindUObject(this, &UTAttack::OnAttackMontageEnd, &OwnerComp);
		currentAnimInstance->Montage_SetEndDelegate(MontageEndDelegate, AttackMontage);

		return EBTNodeResult::InProgress;
	}
	
	/*UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)	return EBTNodeResult::Failed;
	APlayerCharacter* target = Cast<APlayerCharacter>(BlackboardComp->GetValueAsObject(targetKey.SelectedKeyName));
	if (!target)	return EBTNodeResult::Failed;*/

	
	return EBTNodeResult::Failed;
}

void UTAttack::OnAttackMontageEnd(UAnimMontage* PlayedMontage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp) FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}