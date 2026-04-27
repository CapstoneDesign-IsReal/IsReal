// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Elite.h"
#include "AoE_EliteJumpAttack.h"
#include "Components/BoxComponent.h"

AEnemy_Elite::AEnemy_Elite()
{
	ChargeHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ChargeHitBox"));
	ChargeHitBox->SetupAttachment(GetRootComponent());

	LeftClawHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftClawHitBox"));
	LeftClawHitBox->SetupAttachment(GetMesh(), FName("hand_l"));
	RightClawHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightClawHitBox"));
	RightClawHitBox->SetupAttachment(GetMesh(), FName("hand_r"));
}

void AEnemy_Elite::WarnJumpAttack()
{
	if (!JumpAttackAoE) return;

	if (SpawnedAoE) {
		SpawnedAoE->Destroy();
		SpawnedAoE = nullptr;
	}

	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z = 0.0f;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	SpawnedAoE = GetWorld()->SpawnActor<AAoE_EliteJumpAttack>(
		JumpAttackAoE,
		SpawnLocation,
		GetActorRotation(),
		SpawnParams
	);
}

void AEnemy_Elite::ExcuteJumpAttack()
{
	if (!SpawnedAoE) return;

	SpawnedAoE->Explode();
	SpawnedAoE = nullptr;
}