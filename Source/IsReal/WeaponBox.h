// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "WeaponBox.generated.h"

class AWeaponSystem;

UCLASS()
class ISREAL_API AWeaponBox : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AWeaponSystem> ContainedWeapon;

	AWeaponSystem* SpawnedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	FName WeaponSocket;
	FVector SpawnPointLocation;
	FRotator SpawnPointRotation;
	FActorSpawnParameters SpawnParams;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual EInteractionType GetInteractionType_Implementation() override;

	UFUNCTION(BlueprintCallable)
	AWeaponSystem* SpawnWeapon();
};
