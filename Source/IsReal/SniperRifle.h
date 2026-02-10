// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponSystem.h"
#include "Interactable.h"
#include "SniperRifle.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API ASniperRifle : public AWeaponSystem, public IInteractable
{
	GENERATED_BODY()
public:
	ASniperRifle();

	virtual void ResetAmmo() override;

	virtual void WeaponFire() override;
	virtual void WeaponStopFire() override;
	virtual void WeaponReload() override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual EInteractionType GetInteractionType_Implementation() override;
private:
	// 연타 방지용 타이머
	float LastFireTime = -1000.f;
};
