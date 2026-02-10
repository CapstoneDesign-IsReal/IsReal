// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "TChase.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API UTChase : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UTChase(); //Generator

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
public:
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	FBlackboardKeySelector targetKey;
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	FBlackboardKeySelector distanceKey;
};