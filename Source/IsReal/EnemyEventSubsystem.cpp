// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyEventSubsystem.h"

void UEnemyEventSubsystem::EnemyDieNotify(AEnemy* diedEnemy, float TimeEnergy)
{
	if (EnemyDieDelegate.IsBound())	{
		EnemyDieDelegate.Broadcast(TimeEnergy);
	}
	DeleteEnemyfromArray(diedEnemy);
	UE_LOG(LogTemp, Log, TEXT("Enemy Die"));
}  

void UEnemyEventSubsystem::AddEnemytoArray(AEnemy* spawnedEnemy)
{
	AliveEnemyArray.AddUnique(spawnedEnemy);
}

void UEnemyEventSubsystem::DeleteEnemyfromArray(AEnemy* diedEnemy)
{
	AliveEnemyArray.RemoveSwap(diedEnemy);
}

//return true if all enemy die
bool UEnemyEventSubsystem::isAllEnemyDie()
{
	return (AliveEnemyArray.Num() == 0);
}