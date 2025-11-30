// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(Blueprintable)
enum class EnemyState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Chase UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplayName = "Attack")
};

class ISREAL_API EBTEnemyState
{
public:
	EBTEnemyState();
	~EBTEnemyState();
public:
	
	
};
