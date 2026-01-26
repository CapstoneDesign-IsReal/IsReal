// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnemyEventSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FEnemyDieDelegate)

/**
 * 
 */
UCLASS()
class ISREAL_API UEnemyEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	FEnemyDieDelegate EnemyDieDelegate;

	void EnemyDieNotify();
};
