// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Enemy_Tantacle.generated.h"

/**
 * 
 */
UCLASS()
class ISREAL_API AEnemy_Tantacle : public AEnemy
{
	GENERATED_BODY()
	
protected:
	virtual void Hit(int damage, FName HitBoneName) override;

protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	bool isKilled = false;
};
