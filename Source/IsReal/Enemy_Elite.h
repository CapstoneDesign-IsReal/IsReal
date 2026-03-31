// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Enemy_Elite.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class ISREAL_API AEnemy_Elite : public AEnemy
{
	GENERATED_BODY()
public:
	AEnemy_Elite();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UBoxComponent> ChargeHitBox;

public:
	FORCEINLINE UBoxComponent* GetChargeHitBox() const { return ChargeHitBox; }
};
