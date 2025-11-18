// Fill out your copyright notice in the Description page of Project Settings.


#include "TAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerCharacter.h"
#include "AIController.h"
#include "Enemy.h"

UTAttack::UTAttack()
{
	NodeName = "Attack";
}

EBTNodeResult::Type UTAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemy* currentEnemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetCharacter());

	//exception
	if (currentEnemy == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackBoardComp)
		return EBTNodeResult::Failed;

	APlayerCharacter* target = Cast<APlayerCharacter>(BlackBoardComp->GetValueAsObject(targetKey.SelectedKeyName));
	if (!target)
		return EBTNodeResult::Failed;

	currentEnemy->Attack(target);
	return EBTNodeResult::Succeeded;
}