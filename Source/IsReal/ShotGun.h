// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponSystem.h"
#include "ShotGun.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API AShotGun : public AWeaponSystem
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AShotGun();

	virtual void ResetAmmo() override;

	virtual void WeaponFire() override;
	virtual void WeaponStopFire() override;
	virtual void WeaponReload() override;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	int shotgunPellet = 8;
private:
	// 연타 방지용 타이머
	float LastFireTime = -1000.f;

	void ShotGunFireLineTrace();

	UPROPERTY(EditAnywhere, Category = "Weapon Sound")
	USoundBase* PumpSound;

	USkeletalMeshComponent* ShotGunMesh;
};
