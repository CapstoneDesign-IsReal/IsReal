// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Enemy.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemySpawner::SpawnEnemy()
{
	if (!SpawningTarget)	return;

	//Adjust spawn point when Enemy is overlapped with obstacles
	SpawnParams.SpawnCollisionHandlingOverride 
		= ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnPointLocation = GetActorLocation();
	SpawnPointRotation = GetActorRotation();

	AEnemy* spawnedEnemy = GetWorld()->SpawnActor<AEnemy>(
		SpawningTarget,
		SpawnPointLocation,
		SpawnPointRotation,
		SpawnParams
	);

	//setting initial value if needed
	if (spawnedEnemy) {

	}
}
