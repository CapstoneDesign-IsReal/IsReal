// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Tantacle.h"

void AEnemy_Tantacle::Hit(int damage, FName HitBoneName)
{
	HP = HP - damage;
	UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %f"), HP);
	if (HP <= 0)  isKilled = true;
}