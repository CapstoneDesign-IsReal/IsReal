// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController_Boss.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy_Boss.h"
#include "PlayerCharacter.h"

void AEnemyController_Boss::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyController_Boss::DecideAttack()
{
	CurrentEnemy = Cast<AEnemy_Boss>(GetPawn());
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	APlayerCharacter* TargetPlayer = BlackboardComp ? Cast<APlayerCharacter>(BlackboardComp->GetValueAsObject(FName(TEXT("sensedTarget")))) : nullptr;
	if (!CurrentEnemy || !BlackboardComp || !TargetPlayer) return;

	float DistanceToPlayer = CurrentEnemy->GetDistanceTo(TargetPlayer);	
	float TotalWeight = 0.0f;
	float DecisionWeight = 0.0f;

	for (const auto& Pair : AttackMontageList) {
		if (DistanceToPlayer <= Pair.Value.MaxRange + 20.0f)
			TotalWeight += Pair.Value.Preference / (1 + 3 * FMath::Abs(DistanceToPlayer - Pair.Value.MaxRange));
	}

	float RandomRoll = FMath::RandRange(0.0f, TotalWeight);

	for (const auto& Pair : AttackMontageList) {
		if (DistanceToPlayer <= Pair.Value.MaxRange + 20.0f)
			DecisionWeight += Pair.Value.Preference / (1 + 3 * FMath::Abs(DistanceToPlayer - Pair.Value.MaxRange));
		if (DecisionWeight >= RandomRoll) {
			CurrentEnemy->DoAttack(Pair.Key, Pair.Value.Damage);	//Hand over selected animation and info
			break;
		}
	}
}