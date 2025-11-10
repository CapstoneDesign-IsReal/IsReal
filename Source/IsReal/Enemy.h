// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Enemy.generated.h"

UCLASS(Blueprintable)
class ISREAL_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	//Enemy Status
	UPROPERTY(EditAnywhere, Category = "Enemy Params")
	float HP = 200;
	UPROPERTY(EditAnywhere, Category = "Enemy Params")
	float AttackDamage = 100;
	UPROPERTY(EditAnywhere, Category = "Enemy Params")
	float AttackRange = 200.0f;
	UPROPERTY(EditAnywhere, Category = "Enemy Params")
	float AttackCoolTime = 0.0f;	//notify Attack or Not
	UPROPERTY(EditAnywhere, Category = "Enemy Params")
	float AttackSpeed = 10.0f;		//Attack Speed

	FTimerHandle CoolTimeHandle;	//timer handler

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Attack 'target'
	void Attack(APawn* target);
	float getAttackRange();
	float getAttackCoolTime();
	void AttackCountdown();
	void Chase(APawn * target);
};
