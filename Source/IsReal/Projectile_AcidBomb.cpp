// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_AcidBomb.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

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

	if (SphereComp)
		SphereComp->OnComponentHit.AddDynamic(this, &AProjectile_AcidBomb::OnHit);
}

// Called when the game starts or when spawned
void AProjectile_AcidBomb::BeginPlay()
{
	Super::BeginPlay();
	
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
		!OtherComp->IsSimulatingPhysics())	return;



	Destroy();
}

