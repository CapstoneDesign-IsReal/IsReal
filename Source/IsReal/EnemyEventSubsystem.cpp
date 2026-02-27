// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyEventSubsystem.h"

void UEnemyEventSubsystem::EnemyDieNotify(AEnemy* diedEnemy)
{
	if (EnemyDieDelegate.IsBound())	{
		EnemyDieDelegate.Broadcast();
	}
	AliveEnemyArray.RemoveSwap(diedEnemy);
	UE_LOG(LogTemp, Log, TEXT("Enemy Die"));
}  

void UEnemyEventSubsystem::AddEnemyArray(AEnemy* spawnedEnemy)
{
	AliveEnemyArray.AddUnique(spawnedEnemy);
}

//return true if all enemy die
bool UEnemyEventSubsystem::isAllEnemyDie()
{
	return (AliveEnemyArray.Num() == 0);
}