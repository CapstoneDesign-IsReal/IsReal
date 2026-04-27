// Fill out your copyright notice in the Description page of Project Settings.


#include "TBoss_NormalAttack.h"
#include "EnemyController_Boss.h"
#include "Enemy_Boss.h"


UTBoss_NormalAttack::UTBoss_NormalAttack()
{
	NodeName = TEXT("Normal Attack");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UTBoss_NormalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyController_Boss* CurrentController = Cast<AEnemyController_Boss>(OwnerComp.GetAIOwner());
	AEnemy_Boss* CurrentEnemy = CurrentController ? Cast<AEnemy_Boss>(CurrentController->GetCharacter()) : nullptr;
	if (!CurrentController || !CurrentEnemy)	return EBTNodeResult::Failed;

	CachedOwnerComp = &OwnerComp;
	CachedCurrentEnemy = CurrentEnemy;
	CurrentEnemy->BossNormalAttackEnd.RemoveAll(this);
	CurrentEnemy->BossNormalAttackEnd.AddUObject(this, &UTBoss_NormalAttack::OnEndNormalAttack);
	CurrentController->ClearFocus(EAIFocusPriority::Gameplay);
	CurrentController->DecideAttack();
	

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UTBoss_NormalAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UTBoss_NormalAttack::OnEndNormalAttack()
{
	CachedCurrentEnemy->BossNormalAttackEnd.RemoveAll(this);
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}