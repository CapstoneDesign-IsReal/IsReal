// Fill out your copyright notice in the Description page of Project Settings.


#include "TElite_JumpAttack.h"
#include "Enemy_Elite.h"
#include "AIController.h"

UTElite_JumpAttack::UTElite_JumpAttack()
{
	NodeName = "Jump Attack";

	bCreateNodeInstance = true;
}

EBTNodeResult::Type UTElite_JumpAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* CurrentController = Cast<AAIController>(OwnerComp.GetOwner());
	AEnemy_Elite* CurrentEnemy = CurrentController ? Cast<AEnemy_Elite>(CurrentController->GetCharacter()) : nullptr;
	UAnimInstance* CurrentAnim = CurrentEnemy ? Cast<UAnimInstance>(CurrentEnemy->GetMesh()->GetAnimInstance()) : nullptr;
	if (!CurrentController || !CurrentEnemy || !CurrentAnim)	return EBTNodeResult::Failed;

	CurrentController->ClearFocus(EAIFocusPriority::Gameplay);

	CurrentEnemy->PlayAnimMontage(JumpAttackMontage);
	FOnMontageEnded JumpAttackEndDelegate;
	JumpAttackEndDelegate.BindUObject(this, &UTElite_JumpAttack::OnJumpAttackEnd, &OwnerComp);
	CurrentAnim->Montage_SetEndDelegate(JumpAttackEndDelegate, JumpAttackMontage);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UTElite_JumpAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UTElite_JumpAttack::OnJumpAttackEnd(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (bInterrupted) FinishLatentTask(*OwnerComp, EBTNodeResult::Aborted);

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}