// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyHitComponent.generated.h"

class UCapsuleComponent;
class UDA_EnemyPartInfo;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISREAL_API UEnemyHitComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyHitComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "부위별 데미지 | Data Asset")
	TObjectPtr<UDA_EnemyPartInfo> PartInfo;
	UPROPERTY(EditAnywhere, Category = "부위별 데미지 | 기본 배율")
	float DefaultMultiplier = 1.0;

	float DamageProcess(float Damage, FName HitBone);
	float CalDamageMultiplier(FName BoneName);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
