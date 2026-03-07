// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_AcidBomb.h"

// Sets default values
AProjectile_AcidBomb::AProjectile_AcidBomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

