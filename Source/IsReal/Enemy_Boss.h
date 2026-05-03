// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Enemy_Boss.generated.h"

class USphereComponent;

DECLARE_MULTICAST_DELEGATE(FOnAttackEnd);
/**
 * 
 */
UCLASS()
class ISREAL_API AEnemy_Boss : public AEnemy
{
	GENERATED_BODY()
	
public:
	AEnemy_Boss();

	FORCEINLINE USphereComponent* GetLeftHandHitBox() const { return LeftHandHitBox; }
	FORCEINLINE USphereComponent* GetRightHandHitBox() const { return RightHandHitBox; }

	void DoAttack(UAnimMontage* NormalAttackMontage, float SelectedAttackDamage);

protected:
	void NormalAttack(UAnimMontage* AttackMontage, float SelectedAttackDamage);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* AttackMontage, bool bInterrupted);

	void EnableAttackCollision();
	void DisableAttackCollision();

	UFUNCTION()
	void OnHitBoxHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	FOnAttackEnd BossNormalAttackEnd;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<USphereComponent> LeftHandHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<USphereComponent> RightHandHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<USphereComponent> LeftFootHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<USphereComponent> RightFootHitBox;

	float CurrentAttackDamage = 0.0f;

	bool isPlayerHit = false;
};
