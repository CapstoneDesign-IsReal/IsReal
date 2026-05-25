// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TElite_ChargeAttack.generated.h"

class UNiagaraSystem;
class AEnemy_Elite;

UCLASS()
class ISREAL_API UTElite_ChargeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTElite_ChargeAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnPreChargeEnded(UAnimMontage* Montage, bool bInterrupted);
	void EndChargeAttack(UBehaviorTreeComponent& OwnerComp, AEnemy_Elite* CurrentEnemy, UBlackboardComponent* BlackboardComp);

	UFUNCTION()
	void OnChargeHitWall(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnChargeHitPlayer(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	UPROPERTY(EditAnywhere, Category = "Charge Settings")
	float ChargeSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Charge Settings")
	float MaxChargeTime = 10.0f;

	

private:
	UPROPERTY(EditAnywhere)
	float ChargeDamage = 200.0f;

	float CachedOriginalSpeed;
	float ChargeTimer;
	bool isReadyToRun = false;
	bool isChargeEnd = false;
	bool isPlayerHit = false; //prevent multi hit

protected:
	UPROPERTY(EditAnywhere, Category = "Charge Settings")
	UAnimMontage* PreChargeMontage;

	UPROPERTY(EditAnywhere, Category = "Charge Settings | VFX")
	UNiagaraSystem* HitWallVFX;

	UPROPERTY(EditAnywhere, Category = "Charge Settings | Sound")
	USoundBase* CollisionSound;

	UPROPERTY(EditAnywhere, Category = "Charge Settings | VFX")
	FVector SpawnSize = FVector(1.0f, 1.0f, 1.0f);
};