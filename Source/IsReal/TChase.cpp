// Fill out your copyright notice in the Description page of Project Settings.


#include "TChase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerCharacter.h"
#include "AIController.h"
#include "Enemy.h"
#include "EnemyController.h"

//Generator
UTChase::UTChase()
{
	NodeName = "Chase";
}

EBTNodeResult::Type UTChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemy* currentEnemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetCharacter());

	//exception
	if (currentEnemy == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
		return EBTNodeResult::Failed;
	//Find target Actor to chase in BlackBoard
	APlayerCharacter* target = Cast<APlayerCharacter>(BlackboardComp->GetValueAsObject(targetKey.SelectedKeyName));
	if (!target)
		return EBTNodeResult::Failed;

	float dist = BlackboardComp->GetValueAsFloat(distanceKey.SelectedKeyName);
	AEnemy* SelfActor = Cast<AEnemy>(BlackboardComp->GetValueAsObject(TEXT("SelfActor")));

	//do attack
	if (dist < SelfActor->getAttackRange())
	{
		BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Attack));
	}
	else                    //else keep chase
	{
		currentEnemy->Chase(target);
	}
	return EBTNodeResult::Succeeded;
}