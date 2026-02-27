// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

// Sets default values for this component's properties
UCoreSystem::UCoreSystem()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}
// Called when the game starts
void UCoreSystem::BeginPlay()
{
	Super::BeginPlay();

	RewindCore = MaxRewindCore;
	Is_Rewind = false;
	RewindCoolTime = 5.0f; // 총 쿨다운 시간
	CurruntRewindCoolTime = 0.0f; // 현재 쿨다운 시간

	OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	GameInstance = GetWorld()->GetGameInstance();
	World = GetWorld();
	enemyeventsubsys = World->GetSubsystem<UEnemyEventSubsystem>();
}

void UCoreSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCoreSystem::TryReWind() 
{
	FVector CurrentLocation = OwnerCharacter->GetActorLocation();

	if (RewindCore >= 100 && !(GetWorld()->GetTimerManager().IsTimerActive(RewindTimerHandle))) 
	{
		if (Is_Rewind == false)
		{
			GetWorld()->GetTimerManager().SetTimer(RewindTimerHandle, this, &UCoreSystem::RewindCooldown, 1.0f, true);
			// to past
			Is_Rewind = true;
			RewindCore -= 100;
			if (RewindVFX) 
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RewindVFX, 
					CurrentLocation, OwnerCharacter->GetActorRotation());
			}
			FVector NewLocation = CurrentLocation + FVector(0.f, 0.f, 10000.f);
			OwnerCharacter->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
			UE_LOG(LogTemp, Warning, TEXT("Rewind Triggered -> Moved to: %s"), *NewLocation.ToString());
			
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RewindVFX, NewLocation, OwnerCharacter->GetActorRotation());
		}
	}
	else 
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Can't Rewind"), true, true, FLinearColor::Green, 2.0f);
	}
}

void UCoreSystem::RewindCooldown()
{
	CurruntRewindCoolTime++;

	UKismetSystemLibrary::PrintString(						// 쿨다운 보여주는 텍스트(지워도됨)
		GetWorld(),
		FString::Printf(TEXT("CoolDown.. : %.1f"), RewindCoolTime-CurruntRewindCoolTime)
		, true, true, FLinearColor::Green, 2.0f);

	if (CurruntRewindCoolTime >= RewindCoolTime && OwnerCharacter->GetIsDie() == false) {
		GetWorld()->GetTimerManager().ClearTimer(RewindTimerHandle);
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("To Present!"), true, true, FLinearColor::Green, 2.0f);

		// 현재로 올때
		Is_Rewind = false;

		// delegate trigger
		if (GameInstance) 
		{
			UCoreEventSubsystem* coresubsystem = GameInstance->GetSubsystem<UCoreEventSubsystem>();
			if (coresubsystem)
			{
				coresubsystem->RewindDone();
			}
		}

		// 모든 적이 죽었으면 코어 회복
		
		if (enemyeventsubsys)
		{
			if (enemyeventsubsys->isAllEnemyDie())
			{
				CoreHeal();
			}
		}

		OwnerCharacter->UnEquipWeapon(); // 주무기 해제

		CurruntRewindCoolTime = 0.0f;

		FVector CurrentLocation = OwnerCharacter->GetActorLocation();

		FVector NewLocation = CurrentLocation - FVector(0.f, 0.f, 10000.f);
		OwnerCharacter->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RewindVFX, NewLocation, OwnerCharacter->GetActorRotation());
		UE_LOG(LogTemp, Warning, TEXT("Rewind Triggered -> Moved to: %s"), *NewLocation.ToString());
	}
}

void UCoreSystem::RewindOnDeath()   // 죽을 때 현재로 돌아오는 함수 // BP에서 호출
{
	UE_LOG(LogTemp, Warning, TEXT("Rewind On Death"));
	GetWorld()->GetTimerManager().ClearTimer(RewindTimerHandle);
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("To Present!"), true, true, FLinearColor::Green, 2.0f);

	// 현재로 올때
	Is_Rewind = false;

	// delegate trigger
	if (GameInstance)
	{
		UCoreEventSubsystem* coresubsystem = GameInstance->GetSubsystem<UCoreEventSubsystem>();
		if (coresubsystem)
		{
			coresubsystem->RewindDone();
		}
	}

	OwnerCharacter->UnEquipWeapon(); // 주무기 해제

	CurruntRewindCoolTime = 0.0f;

	FVector CurrentLocation = OwnerCharacter->GetActorLocation();

	FVector NewLocation = CurrentLocation - FVector(0.f, 0.f, 10000.f);
	OwnerCharacter->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RewindVFX, NewLocation, OwnerCharacter->GetActorRotation());
	UE_LOG(LogTemp, Warning, TEXT("Rewind Triggered -> Moved to: %s"), *NewLocation.ToString());
}

void UCoreSystem::CoreHeal() 
{
	RewindCore += 100;
	if (RewindCore >= 300) RewindCore = 300;   // Max Rewind Core 이상으로 안올라가게
}