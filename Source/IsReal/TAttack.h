// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TAttack.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API UTAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UTAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

public:
	//area for key
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	FBlackboardKeySelector targetKey;
};
