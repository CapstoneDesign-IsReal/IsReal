// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyController.h"
#include "PlayerCharacter.h"
#include "HealthComponent.h"
#include "EnemyEventSubsystem.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TeamID = FGenericTeamId(1);
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
	auto player = Cast<APlayerCharacter>(target);

	if (player == nullptr) return;
	UHealthComponent* playerHP = player->FindComponentByClass<UHealthComponent>();

	if (playerHP == nullptr) return;
	playerHP->hit(AttackDamage);
	UE_LOG(LogTemp, Warning, TEXT("Current HP: %f"), playerHP->GetPlayerHP());
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

void AEnemy::Hit(int damage)
{
	auto EnemyController = Cast<AEnemyController>(GetController());
	UBlackboardComponent* BlackboardComp = EnemyController->GetBlackboardComponent();

	if (!EnemyController) {
		UE_LOG(LogTemp, Warning, TEXT("<Controller Unpossessed Error>: No Controller"));
	}
	if (!BlackboardComp) {
		UE_LOG(LogTemp, Warning, TEXT("<Perception Process Error>: No BlackBoard"));
		return;
	}

	HP = HP - damage;

	UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %f"), HP);

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
			EnemyEventSubsystem->EnemyDieNotify(this);
		}
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