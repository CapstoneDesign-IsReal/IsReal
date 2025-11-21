// Fill out your copyright notice in the Description page of Project Settings.


#include "TSDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

UTSDistance::UTSDistance()
{
	NodeName = "Update Distance to Target";
}

void UTSDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* Memory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, Memory, DeltaSeconds);

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	APlayerCharacter* target = Cast<APlayerCharacter>(BlackBoardComp->GetValueAsObject(targetKey.SelectedKeyName));
	if (!target) {
		UE_LOG(LogTemp, Warning, TEXT("<Casting Fail>: There is no Target."));
		return;
	}
	AEnemy* selfActor = Cast<AEnemy>(BlackBoardComp->GetValueAsObject(TEXT("SelfActor")));
	if (!selfActor) {
		UE_LOG(LogTemp, Warning, TEXT("<Casting Fail>: There is no Enemy Movable."));
		return;
	}

	float dist = selfActor->GetDistanceTo(target);
	BlackBoardComp->SetValueAsFloat(distanceKey.SelectedKeyName, dist);
}