// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TBoss_NormalAttack.generated.h"

class AEnemy_Boss;
/**
 * 
 */
UCLASS()
class ISREAL_API UTBoss_NormalAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTBoss_NormalAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnEndNormalAttack();

protected:
	UBehaviorTreeComponent* CachedOwnerComp;
	AEnemy_Boss* CachedCurrentEnemy;
};
