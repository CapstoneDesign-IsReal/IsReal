// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyController.h"
#include "EnemyController_Boss.generated.h"

class UBlackboardComponent;
class AEnemy_Boss;

USTRUCT(BlueprintType)
struct FAttackInfo	//struct for TMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MaxRange = 100.0f;

	UPROPERTY(EditAnywhere)
	float Preference = 1.0f;

	UPROPERTY(EditAnywhere)
	float Damage = 20.0f;
};

/**
 * 
 */
UCLASS()
class ISREAL_API AEnemyController_Boss : public AEnemyController
{
	GENERATED_BODY()
	
public:
	void DecideAttack();
	void Groggy();

protected:
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, Category = "Attack | Montage")
	TMap<TObjectPtr<UAnimMontage>, FAttackInfo> AttackMontageList;

	UPROPERTY()
	TObjectPtr<AEnemy_Boss> CurrentEnemy;

	FRotator RotationToPlayer;
};
