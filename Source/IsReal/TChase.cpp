// Fill out your copyright notice in the Description page of Project Settings.


#include "TChase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerCharacter.h"
#include "AIController.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"

//Generator
UTChase::UTChase()
{
	NodeName = "Chase";
	bNotifyTick = true;
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


	currentEnemy->Chase(target);
	return EBTNodeResult::InProgress;
}

//Can Enemy attack player?
void UTChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemy* currentEnemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetCharacter());
	if (currentEnemy == nullptr)	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	float dist = BlackboardComp->GetValueAsFloat(distanceKey.SelectedKeyName);

	//check arrive to target
	if (dist < 150.0)
	{
		BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Nearby));
		currentEnemy->GetCharacterMovement()->StopMovementImmediately();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	if (BlackboardComp->GetValueAsName(TEXT("state")) == TEXT("Die"))
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}