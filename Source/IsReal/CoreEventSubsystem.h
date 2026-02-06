// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CoreEventSubsystem.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FRewindDoneDelegate);

UCLASS()
class ISREAL_API UCoreEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void RewindDone();
	FRewindDoneDelegate RewindDoneDelegate;
};
