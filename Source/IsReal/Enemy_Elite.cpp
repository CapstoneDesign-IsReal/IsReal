// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Elite.h"
#include "Components/BoxComponent.h"

AEnemy_Elite::AEnemy_Elite()
{
	ChargeHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ChargeHitBox"));
	ChargeHitBox->SetupAttachment(GetRootComponent());
}