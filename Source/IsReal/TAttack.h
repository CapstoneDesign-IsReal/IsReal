// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TAttack.generated.h"


class UAnimMontage;
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
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

public:
	//area for key
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
	FBlackboardKeySelector targetKey;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackMontage;	//set in Behavior Tree Blueprint

	UFUNCTION(BlueprintCallable)
	void OnAttackMontageEnd(UAnimMontage* PlayedMontage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
};
