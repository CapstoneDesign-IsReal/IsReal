// Fill out your copyright notice in the Description page of Project Settings.


#include "Rifle.h"

ARifle::ARifle()
{
	Damage = 15;
	CurrentAmmo = 30;
	MaxAmmo = 30;
}

void ARifle::WeaponFire()
{

}

void ARifle::WeaponReload()
{

}

void ARifle::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Gun Interacted"));
	SetWeaponType(EWeaponType::EWT_Rifle);
	Destroy();
}

EInteractionType ARifle::GetInteractionType_Implementation()
{
	return EInteractionType::Gun;
}