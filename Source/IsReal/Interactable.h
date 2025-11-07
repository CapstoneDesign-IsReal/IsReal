// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Door,
	Gun,
	CCTV,
	None
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class ISREAL_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EInteractionType GetInteractionType(); // Ÿ�� ��ȸ��
};