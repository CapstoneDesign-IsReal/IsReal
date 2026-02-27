// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GenericTeamAgentInterface.h"
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

UENUM()
enum class EEnemyState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Chase UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplayName = "Attack"),
	Die UMETA(DisplayName = "Die"),
	Nearby UMETA(DisplayName = "Nearby")
};

UCLASS(Blueprintable)
class ISREAL_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	void AttackDecision(APawn* target);
	AEnemyController();	//define basic constructor 

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UEnvQuery* DodgeEQS;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName LowHPKeyName = TEXT("isLowHP");
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAIPerceptionComponent* AIPerception;
	virtual FGenericTeamId GetGenericTeamId() const override;

	//Handling Enemy action when Enemy sense  something.
	UFUNCTION(BlueprintCallable)
	void PerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	FAIStimulus CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense);

	UFUNCTION()
	void HandleSensedSight(AActor* Actor);

	UFUNCTION()
	void Dodge();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};