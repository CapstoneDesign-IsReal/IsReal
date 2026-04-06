// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TDecideAttack.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API UTDecideAttack : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard Key")
	FBlackboardKeySelector DistanceKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard Key")
	FBlackboardKeySelector AttackTypeKey;

	UPROPERTY(EditAnywhere, Category = "Possibility | Attack Ranges")
	float ClawMaxRange = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Possibility | Attack Ranges")
	float JumpMaxRange = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Possibility | Attack Ranges")
	float ChargeMaxRange = 1500.0f;
};
