// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TDodge.generated.h"

class UAnimMontage;
/**
 * 
 */
UCLASS()
class ISREAL_API UTDodge : public UBTTaskNode
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector DestinationKey;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* DodgeMontage;	//set in Behavior Tree Blueprint

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
public:
	UTDodge();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	UFUNCTION(BlueprintCallable)
	void OnDodgeMontageEnd(UAnimMontage* PlayedMontage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
};
