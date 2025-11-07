// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyController.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Attack(APawn* target)
{
	AttackCoolTime = AttackSpeed;
	GetWorldTimerManager().SetTimer(CoolTimeHandle, this, &AEnemy::AttackCountdown, 1.0f, true);
}

float AEnemy::getAttackRange()
{
	return AttackRange;
}

float AEnemy::getAttackCoolTime()
{
	return AttackCoolTime;
}

void AEnemy::AttackCountdown()
{
	AttackCoolTime--;
	if (AttackCoolTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(CoolTimeHandle);
	}
}

void AEnemy::Chase(APawn* target)
{
	auto targetlocation = GetTargetLocation(target);
	auto EnemyController = Cast<AEnemyController>(GetController());

	EnemyController->MoveToActor(target, 90.0f);
}