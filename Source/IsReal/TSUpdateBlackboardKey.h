// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "TSUpdateBlackboardKey.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API UTSUpdateBlackboardKey : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UTSUpdateBlackboardKey();

protected:
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	FBlackboardKeySelector stateKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* Memory, float DeltaSeconds) override;
};
