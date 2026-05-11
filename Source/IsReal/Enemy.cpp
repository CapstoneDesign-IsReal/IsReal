// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyController.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthComponent.h"
#include "EnemyEventSubsystem.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CoreSystem.h"
#include "EnemyHitComponent.h"
#include "GenericTeamAgentInterface.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TeamID = FGenericTeamId(1);

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 100.0f;

	EnemyHitComp = CreateDefaultSubobject<UEnemyHitComponent>(TEXT("EnemyHitComponent"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()) {
		EnemyEventSubsystem = GetWorld()->GetSubsystem<UEnemyEventSubsystem>();
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Attack(APawn* target)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack Called"));
	//Attack implement
	APlayerCharacter* Player = Cast<APlayerCharacter>(target);
	if (!Player)	return;

	Player->PlayerHit(AttackDamage);
}

float AEnemy::getAttackRange()
{
	return AttackRange;
}

float AEnemy::getAttackCoolTime()
{
	return AttackCoolTime;
}

void AEnemy::AttackCountdown()
{
	AttackCoolTime--;
	if (AttackCoolTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(CoolTimeHandle);
	}
}

void AEnemy::Chase(AActor* target)
{
	auto EnemyController = Cast<AEnemyController>(GetController());
	EnemyController->MoveToActor(target);
}

void AEnemy::Hit(int damage, FName HitBoneName)
{
	auto EnemyController = Cast<AEnemyController>(GetController());
	APlayerCharacter* CurrentPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UBlackboardComponent* BlackboardComp = EnemyController ? EnemyController->GetBlackboardComponent() : nullptr;

	if (!EnemyController || !BlackboardComp  || !CurrentPlayer) {
		UE_LOG(LogTemp, Warning, TEXT("<Controller Unpossessed Error>: Ptr Access Error"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("HitBone : %s"), *HitBoneName.ToString());	//for debug

	float ProcessedDamage = EnemyHitComp->DamageProcess(damage, HitBoneName);

	HP = HP - ProcessedDamage;

	UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %f"), HP);	//for debug

	//Implement Dodge when 50% HP
	if (HP <= MaxHP / 2.0 && !bIsLowHPTriggered && HP > 0) {  
		bIsLowHPTriggered = true;	//prevent multiple execution

		if (LowHPDelegate.IsBound()) {
			LowHPDelegate.Broadcast();
			UE_LOG(LogTemp, Warning, TEXT("Enemy HP Low Broadcast"));
		}
	}

	//Die process
	if (HP <= 0 && (BlackboardComp->GetValueAsEnum(TEXT("state")) != static_cast<uint8>(EEnemyState::Die))) {
		EnemyController->StopMovement();
		BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Die));

		if (EnemyEventSubsystem) {
			EnemyEventSubsystem->EnemyDieNotify(this, TimeEnergy);
		}

		//Core Heal
		UCoreSystem* PlayerCoreSystem = CurrentPlayer->FindComponentByClass<UCoreSystem>();
		PlayerCoreSystem->CoreHeal(TimeEnergy);
		
	}
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

FGenericTeamId AEnemy::GetGenericTeamId() const
{
	return TeamID;
}