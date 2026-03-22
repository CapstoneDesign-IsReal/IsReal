// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBox.h"
#include "WeaponSystem.h"

// Sets default values
AWeaponBox::AWeaponBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeaponBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EInteractionType AWeaponBox::GetInteractionType_Implementation()
{
	return EInteractionType::Box;
}

AWeaponSystem* AWeaponBox::SpawnWeapon()
{
	if (!ContainedWeapon) {
		UE_LOG(LogTemp, Warning, TEXT("Need to Assign Weapon to Spawn"));
		return nullptr;
	}
	SpawnParams.SpawnCollisionHandlingOverride
		= ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnPointLocation = GetActorLocation();
	SpawnPointRotation = GetActorRotation();

	SpawnedWeapon = GetWorld()->SpawnActor<AWeaponSystem>(
		ContainedWeapon,
		SpawnPointLocation,
		SpawnPointRotation,
		SpawnParams
	);
	UE_LOG(LogTemp, Warning, TEXT("Spawned"));
	return SpawnedWeapon;
}