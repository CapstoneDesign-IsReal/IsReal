// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TElite_JumpAttack.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API UTElite_JumpAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UTElite_JumpAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnJumpAttackEnd(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);

protected:
	UPROPERTY(EditAnywhere, Category = "Jump Attack | Montage")
	UAnimMontage* JumpAttackMontage;

};
