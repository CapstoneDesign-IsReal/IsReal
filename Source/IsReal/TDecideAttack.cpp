// Fill out your copyright notice in the Description page of Project Settings.


#include "TDecideAttack.h"
#include "EnemyController.h"
#include "Enemy_Elite.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UTDecideAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* CurrentController = OwnerComp.GetAIOwner();
	AEnemy_Elite* CurrentEnemy = CurrentController ? Cast<AEnemy_Elite>(CurrentController->GetCharacter()) : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!CurrentController || !CurrentEnemy || !BlackboardComp) return EBTNodeResult::Failed;

	float Distance = BlackboardComp->GetValueAsFloat(DistanceKey.SelectedKeyName);

	float ClawWeight = FMath::Max(0.0f, 1.0f - (Distance / ClawMaxRange));
	float JumpWeight = FMath::Max(0.0f, 1.0f - (Distance / JumpMaxRange));
	float ChargeWeight = FMath::Max(0.0f, 1.0f - (Distance / ChargeMaxRange));

	float TotalWeight = ClawWeight + JumpWeight + ChargeWeight;

	if (TotalWeight <= 0.0f){
		BlackboardComp->SetValueAsEnum(AttackTypeKey.SelectedKeyName, (uint8)EAttackType::None);
		return EBTNodeResult::Failed;
	}

	float RandomRoll = FMath::RandRange(0.0f, TotalWeight);

	TMap<EAttackType, float> AttackWeights;	//TMap for matching AttackType & Weight of Attack(Possibility)
	AttackWeights.Add(EAttackType::Claw, ClawWeight);
	AttackWeights.Add(EAttackType::JumpAttack, JumpWeight);
	AttackWeights.Add(EAttackType::Charge, ChargeWeight);

	EAttackType SelectedAttack = EAttackType::Charge;	//Default
	for (const auto& Pair : AttackWeights)
	{
		RandomRoll -= Pair.Value;

		if (RandomRoll <= 0.0f)
		{
			SelectedAttack = Pair.Key; 
			break;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Elite: Selected Attack: %s"), *UEnum::GetValueAsString(SelectedAttack));
	BlackboardComp->SetValueAsEnum(AttackTypeKey.SelectedKeyName, (uint8)SelectedAttack);
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UTDecideAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}