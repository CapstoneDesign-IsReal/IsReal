// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCharacter.h"
#include "CoreEventSubsystem.h"
#include "HealthComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ISREAL_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UHealthComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    float GetPlayerHP();

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool GetIsInvincible() const { return IsInvincible; }
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetIsInvincible(bool bInvincible) { IsInvincible = bInvincible; }

	void hit(float damage);

	//Player Current HP
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	float PlayerHP = 100.0;

	// 델리게이트용 Subsystem
	UCoreEventSubsystem* coresubsys;
private:

    void heal();
    void ExitCombat();
    void ResetHP();

    float LastHitTime = 0.0f;
    float CombatExitDelay = 5.0f; // 공격을 받은 후 전투 상태에서 벗어나는 시간 (초)

    bool IsInvincible = false;

    APlayerCharacter* player;

    FTimerHandle LastHitTimeHandle; // 마지막 공격 시간을 추적하는 타이머 핸들
    FTimerHandle RegenTimerHandle; // 체력 회복 타이머 핸들
};