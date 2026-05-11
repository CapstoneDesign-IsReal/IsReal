// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "PlayerCharacter.h"
#include "Enemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"



//Constructor
AEnemyController::AEnemyController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);
}

void AEnemyController::AttackDecision(APawn* target)
{
	//AIPerception->GetActorsPerception();
	auto enemyPawn = Cast<AEnemy>(GetPawn());
	auto dist = FVector::Dist2D(
		target->GetActorLocation(),
		enemyPawn->GetActorLocation());

	if (enemyPawn) {	//Defensive
		if (dist < enemyPawn->getAttackRange()
			&& enemyPawn->getAttackCoolTime()) {
			enemyPawn->Attack(target);
		}
	}
}

//Handling when Enemy sense something
void AEnemyController::PerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* UpdatedActor : UpdatedActors)
	{
		FAIStimulus AIStimulus;
		AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Sight);
		if (AIStimulus.WasSuccessfullySensed()) //if Enemy Sense something by sight
		{
			UE_LOG(LogTemp, Warning, TEXT("Chase Start"));
			//MoveToActor(UpdatedActor);
			HandleSensedSight(UpdatedActor);
		}
		AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Hearing);
		if (AIStimulus.WasSuccessfullySensed()) //if Enemy Sense something by Hearing
		{
			//do something
		}
		AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Damage);
		if (AIStimulus.WasSuccessfullySensed()) //if Enemy Sense something by Damage
		{
			//do something
			UE_LOG(LogTemp, Warning, TEXT("EnemyController: Damage Sensed"));
			HandleSensedDamage(UpdatedActor);
		}
	}
}

//Check getActorPerception
FAIStimulus AEnemyController::CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense)
{
	//contain Percption information
	FActorPerceptionBlueprintInfo ActorPerceptionBlueprintInfo;
	FAIStimulus ResultStimulus;

	AIPerception->GetActorsPerception(Actor, ActorPerceptionBlueprintInfo);

	TSubclassOf<UAISense> QuerySenseClass;

	switch (AIPerceptionSense)
	{
	case EAIPerceptionSense::EPS_None:
		break;
	case EAIPerceptionSense::EPS_Sight:
		QuerySenseClass = UAISense_Sight::StaticClass();
		break;
	case EAIPerceptionSense::EPS_Hearing:
		QuerySenseClass = UAISense_Hearing::StaticClass();
		break;
	case EAIPerceptionSense::EPS_Damage:
		QuerySenseClass = UAISense_Damage::StaticClass();
		break;
	case EAIPerceptionSense::EPS_MAX:
		break;
	default:
		break;
	}
	TSubclassOf<UAISense> LastSensedStimulusClass;

	for (const FAIStimulus& AIStimulus : ActorPerceptionBlueprintInfo.LastSensedStimuli)
	{
		LastSensedStimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, AIStimulus);


		if (QuerySenseClass == LastSensedStimulusClass)
		{
			ResultStimulus = AIStimulus;
			return ResultStimulus;
		}

	}
	return ResultStimulus;
}

void AEnemyController::HandleSensedSight(AActor* Actor)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!BlackboardComp) {
		UE_LOG(LogTemp, Warning, TEXT("<Perception Process Error>: No BlackBoard"));
		return;
	}
	//Setting Blackboard Key value to Move Enemy
	APlayerCharacter* target = Cast<APlayerCharacter>(Actor);
	if (target) {
		BlackboardComp->SetValueAsObject(TEXT("sensedTarget"), Actor);
		BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Chase));
	}
	else
	{
		return;
	}
}

void AEnemyController::HandleSensedDamage(AActor* Actor)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!BlackboardComp) {
		UE_LOG(LogTemp, Warning, TEXT("<Perception Process Error>: No BlackBoard"));
		return;
	}
	//Setting Blackboard Key value to Move Enemy
	APlayerCharacter* target = Cast<APlayerCharacter>(Actor);
	if (target) {
		BlackboardComp->SetValueAsObject(TEXT("sensedTarget"), Actor);
		BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Chase));
	}
	else
	{
		return;
	}
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemy* EnemyPawn = Cast<AEnemy>(InPawn);
	if (EnemyPawn) {
		EnemyPawn->LowHPDelegate.RemoveDynamic(this, &AEnemyController::Dodge);
		EnemyPawn->LowHPDelegate.AddDynamic(this, &AEnemyController::Dodge);

		UE_LOG(LogTemp, Log, TEXT("Success binding Dodge"));
	}
}

void AEnemyController::OnUnPossess()
{
	AEnemy* EnemyPawn = Cast<AEnemy>(GetPawn());
	if (EnemyPawn) {
		EnemyPawn->LowHPDelegate.RemoveDynamic(this, &AEnemyController::Dodge);

		UE_LOG(LogTemp, Log, TEXT("Success Remove Binding"));
	}

	Super::OnUnPossess();
}

void AEnemyController::Dodge()
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	BlackboardComp->SetValueAsBool(LowHPKeyName, true);
}

FGenericTeamId AEnemyController::GetGenericTeamId() const
{
	const IGenericTeamAgentInterface* PawnAsTeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn());
	if (!PawnAsTeamAgent)	return FGenericTeamId::NoTeam;

	return PawnAsTeamAgent->GetGenericTeamId();
}

ETeamAttitude::Type AEnemyController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* sensedTeamAgent = Cast<IGenericTeamAgentInterface>(&Other);
	if (!sensedTeamAgent)	return ETeamAttitude::Neutral;
	AEnemy* EnemyPawn = Cast<AEnemy>(GetPawn());
	if(!EnemyPawn) return ETeamAttitude::Neutral;

	if (sensedTeamAgent->GetGenericTeamId() == EnemyPawn->GetGenericTeamId())
		return ETeamAttitude::Friendly;
	else {
		return ETeamAttitude::Hostile;
	}
}

void AEnemyController::ReportDamaged(float Damage)
{
	AEnemy* ControlledPawn = Cast<AEnemy>(GetPawn());
	APlayerCharacter* Attacker = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (!ControlledPawn || !Attacker)
	{
		return; 
	}

	UAISense_Damage::ReportDamageEvent(
		GetWorld(),
		ControlledPawn,
		Attacker,
		Damage,
		Attacker->GetActorLocation(),
		ControlledPawn->GetActorLocation()
	);
}