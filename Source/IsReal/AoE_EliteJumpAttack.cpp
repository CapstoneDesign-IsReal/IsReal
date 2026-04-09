// Fill out your copyright notice in the Description page of Project Settings.


#include "AoE_EliteJumpAttack.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraSystem.h"
#include "PlayerCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AAoE_EliteJumpAttack::AAoE_EliteJumpAttack()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	WarningDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("WarningDecal"));
	WarningDecal->SetupAttachment(RootComponent);
	WarningDecal->DecalSize = FVector(10.0f, DamageRadius, DamageRadius);
	WarningDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

	ExplosionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionSphere"));
	ExplosionSphere->SetupAttachment(RootComponent);
	ExplosionSphere->InitSphereRadius(DamageRadius);

	ExplosionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAoE_EliteJumpAttack::BeginPlay()
{
	Super::BeginPlay();
	
	WarningDecal->SetVisibility(true);
}

void AAoE_EliteJumpAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAoE_EliteJumpAttack::Explode()
{
	WarningDecal->SetVisibility(false);

	ExplosionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExplosionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExplosionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	if (!ExplosionVFX) {
		UE_LOG(LogTemp, Warning, TEXT("AAoE_EliteJumpAttack : No VFX"))
		return;
	}

	APlayerCharacter* CurrentPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (CurrentPlayer && ExplosionSphere->IsOverlappingActor(CurrentPlayer)) {
		CurrentPlayer->PlayerHit(ExplosionDamage);
	}

	FVector SpawnLocation = GetActorLocation();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		ExplosionVFX,
		SpawnLocation,
		FRotator::ZeroRotator,
		FVector(1.0f),
		true,
		true);

	SetLifeSpan(2.0f); 
}
