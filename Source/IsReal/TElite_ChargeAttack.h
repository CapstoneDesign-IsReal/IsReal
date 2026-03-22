// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TElite_ChargeAttack.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API UTElite_ChargeAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* Pre_ChargeMontage;	//set in Behavior Tree Blueprint
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* ChargeMontage;	//set in Behavior Tree Blueprint

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
