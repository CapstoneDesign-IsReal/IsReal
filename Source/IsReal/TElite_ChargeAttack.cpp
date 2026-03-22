// Fill out your copyright notice in the Description page of Project Settings.


#include "TElite_ChargeAttack.h"
#include "AIController.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "Animation/AnimMontage.h"

EBTNodeResult::Type UTElite_ChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UTElite_ChargeAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyController* currentController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if (!currentController)	return EBTNodeResult::Failed;
	AEnemy* currentEnemy = Cast<AEnemy>(currentController->GetCharacter());
	if (currentEnemy == nullptr)	return EBTNodeResult::Failed;
	UAnimInstance* currentAnimInstance = currentEnemy->GetMesh()->GetAnimInstance();
	if (!currentAnimInstance)	return EBTNodeResult::Failed;

	currentAnimInstance->Montage_Stop(0.2f, Pre_ChargeMontage);
	currentAnimInstance->Montage_Stop(0.2f, ChargeMontage);

	return Super::AbortTask(OwnerComp, NodeMemory);
}