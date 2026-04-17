// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_EnemyPartInfo.generated.h"


USTRUCT(BlueprintType)
struct FPartInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageMultiplier;

	//initialize structure
	FPartInfo()
		: DamageMultiplier(1.0f) 
	{}
};

/**
 * 
 */
UCLASS()
class ISREAL_API UDA_EnemyPartInfo : public UDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TMap<FName, FPartInfo> BoneMap;

public:
	const FPartInfo* GetPartInfoByBoneName(FName BoneName);
};
