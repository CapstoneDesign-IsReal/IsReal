// Fill out your copyright notice in the Description page of Project Settings.


#include "EQC_TargetPlayerMovingVector.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Direction.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UEQC_TargetPlayerMovingVector::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* currentPlayer = UGameplayStatics::GetPlayerPawn(QueryInstance.World, 0);

	if (currentPlayer) {
		FVector Velocity = currentPlayer->GetVelocity();

		Velocity.Z = 0.0f;

		if (!Velocity.IsNearlyZero())
		{
			Velocity.Normalize();
		}

		UEnvQueryItemType_Direction::SetContextHelper(ContextData, Velocity);
	}
}
