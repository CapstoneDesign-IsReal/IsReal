// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemyEventSubsystem.generated.h"

class AEnemy;

DECLARE_MULTICAST_DELEGATE(FEnemyDieDelegate);
/**
 * 
 */
UCLASS()
class ISREAL_API UEnemyEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	FEnemyDieDelegate EnemyDieDelegate;	//for external class

	void EnemyDieNotify(AEnemy* diedEnemy);
	void AddEnemyArray(AEnemy* spawnedEnemy);
	bool isAllEnemyDie();
private:
	UPROPERTY()
	TArray<AEnemy*> AliveEnemyArray;
};
