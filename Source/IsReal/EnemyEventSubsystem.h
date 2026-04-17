// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyEventSubsystem.generated.h"

class AEnemy;

DECLARE_MULTICAST_DELEGATE_OneParam(FEnemyDieDelegate, float);
DECLARE_MULTICAST_DELEGATE(FPlayerDieDelegate);
/**
 * 
 */
UCLASS()
class ISREAL_API UEnemyEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	FEnemyDieDelegate EnemyDieDelegate;	//for external class
	FPlayerDieDelegate PlayerDieDelegate;

	void EnemyDieNotify(AEnemy* diedEnemy, float TimeEnergy);
	void PlayerDieNotifyToEnemy();
	void AddEnemytoArray(AEnemy* spawnedEnemy);
	void DeleteEnemyfromArray(AEnemy* diedEnemy);
	bool isAllEnemyDie();
private:
	UPROPERTY()
	TArray<AEnemy*> AliveEnemyArray;
};
