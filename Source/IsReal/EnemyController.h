// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy.h"
#include "EnemyController.generated.h"

/**
 *
 */

 //enum class to know Enemy's perception state
UENUM()
enum class EAIPerceptionSense : uint8
{
	EPS_None UMETA(DisplayName = "None"),
	EPS_Sight UMETA(DisplayName = "Sight"),
	EPS_Hearing UMETA(DisplayName = "Hearing"),
	EPS_Damage UMETA(DisplayName = "Damage"),
	EPS_MAX UMETA(DisplayName = "MAX")
};

UCLASS(Blueprintable)
class ISREAL_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	void AttackDecision(APawn* target);
	AEnemyController();	//define basic constructor 
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAIPerceptionComponent* AIPerception;

	//Handling Enemy action when Enemy sense  something.
	UFUNCTION(BlueprintCallable)
	void PerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	FAIStimulus CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense);
};