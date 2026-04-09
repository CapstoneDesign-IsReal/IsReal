// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Enemy_Elite.generated.h"

class UBoxComponent;
class AAoE_EliteJumpAttack;
/**
 * 
 */
UCLASS()
class ISREAL_API AEnemy_Elite : public AEnemy
{
	GENERATED_BODY()

public:
	AEnemy_Elite();

	FORCEINLINE UBoxComponent* GetChargeHitBox() const { return ChargeHitBox; }
	FORCEINLINE UBoxComponent* GetLeftClawHitBox() const { return LeftClawHitBox; }
	FORCEINLINE UBoxComponent* GetRightClawHitBox() const { return RightClawHitBox; }

	void WarnJumpAttack();
	void ExcuteJumpAttack();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UBoxComponent> ChargeHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UBoxComponent> LeftClawHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UBoxComponent> RightClawHitBox;
	
	UPROPERTY(EditDefaultsOnly, Category = "JumpAttack | AoE")
	TSubclassOf<AAoE_EliteJumpAttack> JumpAttackAoE;

	
	UPROPERTY(Transient)
	TObjectPtr<AAoE_EliteJumpAttack> SpawnedAoE;	//for caching AoE
};
