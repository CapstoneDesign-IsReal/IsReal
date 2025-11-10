// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystem.h"

// Sets default values
AWeaponSystem::AWeaponSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeaponSystem::BeginPlay()
{
	Super::BeginPlay();
	SetWeaponType(EWeaponType::EWT_None); // Initial Type
}

// Called every frame
void AWeaponSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponSystem::WeaponFire()
{
	// Implement in child classes
}
void AWeaponSystem::WeaponReload()
{
	// Implement in child classes
}

// Weapon Type setters and getters
void AWeaponSystem::SetWeaponType(EWeaponType NewType) { _weapontype = NewType; }

EWeaponType AWeaponSystem::GetWeaponType() const { return _weapontype; }


