// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_AcidBomb.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraSystem.h"
#include "PlayerCharacter.h"
#include "AoE_Acid.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AProjectile_AcidBomb::AProjectile_AcidBomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	RootComponent = SphereComp;
	ProjectileMovementComp->SetUpdatedComponent(SphereComp);
	ProjectileMeshComp->SetupAttachment(SphereComp);

	ExplosionSize = FVector(1.f, 1.f, 1.f);
	ExplosionRotation = FRotator::ZeroRotator;
}

// Called when the game starts or when spawned
void AProjectile_AcidBomb::BeginPlay()
{
	Super::BeginPlay();
	
	if (SphereComp)
		SphereComp->OnComponentHit.AddDynamic(this, &AProjectile_AcidBomb::OnHit);
}

// Called every frame
void AProjectile_AcidBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile_AcidBomb::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == nullptr ||
		OtherActor == this ||
		OtherComp == nullptr ||
		OtherActor == GetInstigator() ||
		OtherActor == GetOwner())	return;

	if (APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(OtherActor)) {
		HitPlayer->PlayerHit(Damage);
	}

	FVector HitLocation = Hit.Location;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, HitLocation, ExplosionRotation, ExplosionSize, true, true);

	FVector SpawnPointLocation = GetActorLocation();
	SpawnPointLocation.Z = 0.0f;
	FRotator SpawnPointRotation = FRotator::ZeroRotator; //fix rotator paralle to ground

	AAoE_Acid* SpawnedAoE  = GetWorld()->SpawnActor<AAoE_Acid>(
		SpawnTargetAoE,
		SpawnPointLocation,
		SpawnPointRotation
	);

	Destroy();
}

