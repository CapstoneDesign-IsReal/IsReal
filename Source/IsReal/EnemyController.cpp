// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"



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
	BlackboardComp->SetValueAsObject(TEXT("sensedTarget"), Actor);
	BlackboardComp->SetValueAsEnum(TEXT("state"), static_cast<uint8>(EEnemyState::Chase));
}
