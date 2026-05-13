// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Boss.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.h"
#include "EnemyController_Boss.h"


AEnemy_Boss::AEnemy_Boss()
{
	LeftHandHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHandHitBox"));
	LeftHandHitBox->SetupAttachment(GetMesh(), FName("hand_l"));
	RightHandHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandHitBox"));
	RightHandHitBox->SetupAttachment(GetMesh(), FName("hand_r"));
	LeftFootHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("LeftFootHitBox"));
	LeftFootHitBox->SetupAttachment(GetMesh(), FName("foot_l"));
	RightFootHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("RightFootHitBox"));
	RightFootHitBox->SetupAttachment(GetMesh(), FName("foot_r"));

	PrimaryActorTick.bCanEverTick = false;
}

void AEnemy_Boss::DoAttack(UAnimMontage* NormalAttackMontage, float SelectedAttackDamage)
{
	if (!NormalAttackMontage) return; //implement SpecialAttack

	NormalAttack(NormalAttackMontage, SelectedAttackDamage);
}

void AEnemy_Boss::NormalAttack(UAnimMontage* AttackMontage, float SelectedAttackDamage)
{
	isPlayerHit = false;
	EnableAttackCollision();	//Enable Damage to Player

	CurrentAttackDamage = SelectedAttackDamage;
	float Duration = PlayAnimMontage(AttackMontage);

	//Handling when fail to play Montage
	if (Duration <= 0.0f) {
		OnAttackMontageEnded(AttackMontage, true);
		return;
	}

	FOnMontageEnded AttackMonatageEnd;
	AttackMonatageEnd.BindUObject(this, &AEnemy_Boss::OnAttackMontageEnded);
	GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(AttackMonatageEnd, AttackMontage);
}

void AEnemy_Boss::OnAttackMontageEnded(UAnimMontage* AttackMontage, bool bInterrupted)
{
	if (BossNormalAttackEnd.IsBound())
	{
		BossNormalAttackEnd.Broadcast();
	}
	DisableAttackCollision();
}

void AEnemy_Boss::EnableAttackCollision()
{
	LeftHandHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightHandHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftFootHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightFootHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	LeftHandHitBox->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	RightHandHitBox->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	LeftFootHitBox->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	RightFootHitBox->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	
	LeftHandHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	RightHandHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	LeftFootHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	RightFootHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Boss::OnHitBoxHit);
}

void AEnemy_Boss::DisableAttackCollision()
{
	LeftHandHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftFootHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightFootHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LeftHandHitBox->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	RightHandHitBox->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	LeftFootHitBox->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy_Boss::OnHitBoxHit);
	RightFootHitBox->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy_Boss::OnHitBoxHit);
}

void AEnemy_Boss::OnHitBoxHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (isPlayerHit) return;
	isPlayerHit = true;
	APlayerCharacter* CurrentPlayer = OtherActor ? Cast<APlayerCharacter>(OtherActor) : nullptr;
	if (!OtherComp || !OtherActor || OtherActor == OverlappedComponent->GetOwner() || !CurrentPlayer) return;

	CurrentPlayer->PlayerHit(CurrentAttackDamage);
}

void AEnemy_Boss::Hit(int damage, FName HitBoneName)
{
	if (GetIsArmored()) { //No damage when armor is active.
		UE_LOG(LogTemp, Warning, TEXT("Enemy_Boss : Boss is Armored"), HP);
		return; 
	}
	UE_LOG(LogTemp, Warning, TEXT("Boss HP : %f"), HP);
	Super::Hit(damage, HitBoneName);
}

void AEnemy_Boss::DecreaseArmor()
{
	Armor = Armor - 50.0f;
	
	if (Armor <= 0) {
		isArmored = false;
		Cast<AEnemyController_Boss>(GetController())->Groggy();
	}
}