// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHitComponent.h"
#include "Components/CapsuleComponent.h"
#include "DA_EnemyPartInfo.h"
#include "Enemy.h"

// Sets default values for this component's properties
UEnemyHitComponent::UEnemyHitComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyHitComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEnemyHitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UEnemyHitComponent::DamageProcess(int Damage, FName HitBone)
{
	return Damage * CalDamageMultiplier(HitBone);
}

float UEnemyHitComponent::CalDamageMultiplier(FName BoneName)
{
	FName TargetBone = BoneName;
	AEnemy* CurrentEnemy = Cast<AEnemy>(GetOwner());
	USkeletalMeshComponent* MeshComp = CurrentEnemy->FindComponentByClass<USkeletalMeshComponent>();
	const FPartInfo* BoneInfo;

	if (!CurrentEnemy || !MeshComp || !PartInfo) {
		UE_LOG(LogTemp, Warning, TEXT("Cast Failed"));
		return 0;
	}

	while (!TargetBone.IsNone())
	{
		BoneInfo = PartInfo->GetPartInfoByBoneName(TargetBone);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *TargetBone.ToString());
		if(BoneInfo != nullptr)
			return BoneInfo->DamageMultiplier;

		TargetBone = MeshComp->GetParentBone(TargetBone);
	}

	return DefaultMultiplier;
}