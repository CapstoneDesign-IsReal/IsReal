// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponSystem.h"
#include "Interactable.h"
#include "Rifle.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API ARifle : public AWeaponSystem, public IInteractable
{
	GENERATED_BODY()

public:
	ARifle();

	virtual void WeaponFire() override;
	virtual void WeaponReload() override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual EInteractionType GetInteractionType_Implementation() override;
};
