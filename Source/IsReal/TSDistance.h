// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "TSDistance.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API UTSDistance : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UTSDistance();

	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	FBlackboardKeySelector targetKey;
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	FBlackboardKeySelector distanceKey;
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* Memory, float DeltaSeconds) override;
};
