// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyEventSubsystem.h"
#include "CoreEventSubsystem.h"
#include "EnemySpawner.generated.h"

class AEnemy;

/**
 *
 */
UCLASS()
class ISREAL_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AEnemy> SpawningTarget;

	FVector SpawnPointLocation;
	FRotator SpawnPointRotation;

	FActorSpawnParameters SpawnParams;

	UEnemyEventSubsystem* EnemyEventSubsystem;
	UCoreEventSubsystem* CoreEventSubsystem;

	AEnemy* SpawnedEnemy;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();
	void InitialSetting(AEnemy* spawned);
	void ResetEnemy();
};
