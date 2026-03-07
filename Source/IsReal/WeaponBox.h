// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"

class AWeaponSystem;

#include "WeaponBox.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual EInteractionType GetInteractionType_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Box")
	//TSubclassOf<AWeaponSystem> ContainedWeapon;
	AWeaponSystem* ContainedWeapon;
};
