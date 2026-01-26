// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyEventSubsystem.h"

void UEnemyEventSubsystem::EnemyDieNotify()
{
	if (EnemyDieDelegate.IsBound())
	{
		EnemyDieDelegate.Broadcast();
	}
	UE_LOG(LogTemp, Log, TEXT("Enemy Die"));
}  