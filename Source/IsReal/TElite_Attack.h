// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TElite_Attack.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class ISREAL_API UTElite_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTElite_Attack();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);
	
	UFUNCTION()
	void OnPlayerHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(EditAnywhere, Category = "Attack | Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Attack | Params")
	float Damage = 30.0f;

	UPROPERTY()
	UBoxComponent* LeftClawHitBox;

	UPROPERTY()
	UBoxComponent* RightClawHitBox;

	bool isHitPlayer = false;
};
