// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"

#include "Card.generated.h"

UCLASS()
class ISREAL_API ACard : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	ACard();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Card")
	UStaticMeshComponent* CardMesh;

	
	virtual void Interact_Implementation(AActor* Interactor)override;
	virtual EInteractionType GetInteractionType_Implementation()override;
};