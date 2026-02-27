// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCharacter.h"
#include "CoreEventSubsystem.h"
#include "EnemyEventSubsystem.h"
#include "CoreSystem.generated.h"

class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISREAL_API UCoreSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCoreSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UFUNCTION(BlueprintCallable, Category = "CoreSystem")
	int GetRewindCore() const { return RewindCore; }

	UFUNCTION(BlueprintCallable, Category = "CoreSystem")
	void TryReWind();

	UFUNCTION(BlueprintCallable, Category = "CoreSystem")
	void RewindOnDeath();

	// 과거 체류시간 getter, setter
	UFUNCTION(BlueprintCallable, Category = "CoreSystem")
	float GetRewindCoolTime() { return RewindCoolTime; }
	UFUNCTION(BlueprintCallable, Category = "CoreSystem")
	void SetRewindCoolTime(float CoolTime) { RewindCoolTime = CoolTime; }
private:
	// Rewind State
	bool Is_Rewind = false;
	// Rewind Core
	int RewindCore = 300;
	// Max Rewind Core
	int MaxRewindCore = 300;
	// Rewind Cooldown
	float RewindCoolTime;
	float CurruntRewindCoolTime;
	FTimerHandle RewindTimerHandle;
	// Rewind Cooldown function
	void RewindCooldown();
	// Core Heal
	void CoreHeal();

	// Rewind Effect
	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* RewindVFX;

	// Owner Character
	APlayerCharacter* OwnerCharacter;

	UGameInstance* GameInstance;
	
	// enemy event subsystem
	UWorld* World;
	UEnemyEventSubsystem* enemyeventsubsys;
};
