// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AoE_EliteJumpAttack.generated.h"

class USceneComponent;
class UDecalComponent;
class USphereComponent;
class UNiagaraSystem;

UCLASS()
class ISREAL_API AAoE_EliteJumpAttack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAoE_EliteJumpAttack();

	void Explode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AoE")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AoE")
	TObjectPtr<UDecalComponent> WarningDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AoE")
	TObjectPtr<USphereComponent> ExplosionSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AoE | VFX")
	TObjectPtr<UNiagaraSystem> ExplosionVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AoE | VFX")
	FVector ExplosionSize = FVector(1.0f, 1.0f, 1.0f);;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AoE")
	float DamageRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AoE")
	float ExplosionDamage = 30.0f;

};
