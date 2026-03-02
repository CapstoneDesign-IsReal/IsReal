// Fill out your copyright notice in the Description page of Project Settings.


#include "TDodge.h"

UTDodge::UTDodge()
{
	NodeName = "Dodge";
}

EBTNodeResult::Type UTDodge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyController* currentController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* currentBlackBoardComp = OwnerComp.GetBlackboardComponent();
	FVector destination = currentBlackBoardComp->GetValueAsVector(DestinationKey.SelectedKeyName);
	if (!currentController) return EBTNodeResult::Failed;
	if (!currentBlackBoardComp) return EBTNodeResult::Failed;

	currentController->MoveToLocation(destination);
	
	return EBTNodeResult::Failed;
}

void UTDodge::ExceptionCheck()
{

}