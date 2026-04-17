// Fill out your copyright notice in the Description page of Project Settings.


#include "TDodge.h"
#include "Animation/AnimMontage.h"

UTDodge::UTDodge()
{
	NodeName = "Dodge";
}

EBTNodeResult::Type UTDodge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyController* currentController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* currentBlackboardComp = OwnerComp.GetBlackboardComponent();
	FVector destination = currentBlackboardComp->GetValueAsVector(DestinationKey.SelectedKeyName);
	AEnemy* currentEnemy = Cast<AEnemy>(currentController->GetCharacter());
	UAnimInstance* currentAnimInstance = currentEnemy->GetMesh()->GetAnimInstance();
	if (!currentController ||
		!currentBlackboardComp ||
		!currentEnemy ||
		!currentAnimInstance ||
		!DodgeMontage) return EBTNodeResult::Failed;

	currentController->MoveToLocation(destination);
	currentEnemy->PlayAnimMontage(DodgeMontage);

	//Stop Attack Motion immediately when Enemy die
	if (currentBlackboardComp->GetValueAsName(TEXT("state")) == TEXT("Die")) {
		if (DodgeMontage)
			currentEnemy->StopAnimMontage(DodgeMontage);
		return EBTNodeResult::Succeeded;
	}

	FOnMontageEnded MontageEndDelegate;
	MontageEndDelegate.BindUObject(this, &UTDodge::OnDodgeMontageEnd, &OwnerComp);
	currentAnimInstance->Montage_SetEndDelegate(MontageEndDelegate, DodgeMontage);

	return EBTNodeResult::InProgress;
}

void UTDodge::OnDodgeMontageEnd(UAnimMontage* PlayedMontage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	//UBlackboardComponent* BlackboardComp = OwnerComp->GetBlackboardComponent();
	//if(!BlackboardComp) FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);


	if (OwnerComp) {
		//BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Chase));
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UTDodge::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AEnemyController* currentController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	AEnemy* currentEnemy = Cast<AEnemy>(currentController->GetCharacter());
	UAnimInstance* currentAnimInstance = currentEnemy->GetMesh()->GetAnimInstance();
	if (!currentController ||
		currentEnemy == nullptr ||
		!currentAnimInstance)	return EBTNodeResult::Failed;

	currentAnimInstance->Montage_Stop(0.2f, DodgeMontage);

	return Super::AbortTask(OwnerComp, NodeMemory);
}
