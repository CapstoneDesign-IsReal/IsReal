// Fill out your copyright notice in the Description page of Project Settings.


#include "TSUpdateBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyController.h"

UTSUpdateBlackboardKey::UTSUpdateBlackboardKey()
{
	NodeName = "Update Enemy State";
}

void UTSUpdateBlackboardKey::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* Memory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, Memory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	BlackboardComp->SetValueAsEnum(stateKey.SelectedKeyName, static_cast<uint8>(EEnemyState::Chase));
}