// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_JumpAttack.h"
#include "Enemy_Elite.h"

void UANS_JumpAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	AEnemy_Elite* CurrentEnemy = MeshComp ? Cast<AEnemy_Elite>(MeshComp->GetOwner()) : nullptr;
	if (!CurrentEnemy) return;

	CurrentEnemy->WarnJumpAttack();
}

void UANS_JumpAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AEnemy_Elite* CurrentEnemy = MeshComp ? Cast<AEnemy_Elite>(MeshComp->GetOwner()) : nullptr;
	if (!CurrentEnemy) return;

	CurrentEnemy->ExcuteJumpAttack();
}