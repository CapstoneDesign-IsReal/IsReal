// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
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
	CurruntRewindCoolTime = 0.0f; // ���� ��ٿ� �ð�

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
	if (!OwnerCharacter) OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (OwnerCharacter == nullptr) return;
	FVector CurrentLocation = OwnerCharacter->GetActorLocation();

	if (RewindCore >= 100 && !(GetWorld()->GetTimerManager().IsTimerActive(RewindTimerHandle)) && GetIsEnterBoss() == false)
	{
		if (Is_Rewind == false)
		{
			GetWorld()->GetTimerManager().SetTimer(RewindTimerHandle, this, &UCoreSystem::RewindCooldown, 1.0f, true);
			// to past
			Is_Rewind = true;
			if (OwnerCharacter)
			{
				OwnerCharacter->StartAfterImage();
			}
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
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Can't Rewind, Game Over"), true, true, FLinearColor::Green, 2.0f);
	}
}

void UCoreSystem::RewindCooldown()
{
	CurruntRewindCoolTime++;

	UKismetSystemLibrary::PrintString(						// ��ٿ� �����ִ� �ؽ�Ʈ(��������)
		GetWorld(),
		FString::Printf(TEXT("CoolDown.. : %.1f"), RewindCoolTime-CurruntRewindCoolTime)
		, true, true, FLinearColor::Green, 2.0f);

	if (CurruntRewindCoolTime >= RewindCoolTime && OwnerCharacter->GetIsDie() == false) {
		GetWorld()->GetTimerManager().ClearTimer(RewindTimerHandle);
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("To Present!"), true, true, FLinearColor::Green, 2.0f);

		// ����� �ö�
		Is_Rewind = false;
		if (OwnerCharacter)
		{
			OwnerCharacter->StopAfterImage();
		}
		// delegate trigger
		if (GameInstance) 
		{
			UCoreEventSubsystem* coresubsystem = GameInstance->GetSubsystem<UCoreEventSubsystem>();
			if (coresubsystem)
			{
				coresubsystem->RewindDone();
			}
		}

		OwnerCharacter->UnEquipWeapon(); // �ֹ��� ����

		CurruntRewindCoolTime = 0.0f;

		FVector CurrentLocation = OwnerCharacter->GetActorLocation();

		FVector NewLocation = CurrentLocation - FVector(0.f, 0.f, 10000.f);
		OwnerCharacter->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RewindVFX, NewLocation, OwnerCharacter->GetActorRotation());
		UE_LOG(LogTemp, Warning, TEXT("Rewind Triggered -> Moved to: %s"), *NewLocation.ToString());

		// if core is less than 100, game is over
		if (RewindCore < 100) {
			UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Can't Rewind, Game Over"), true, true, FLinearColor::Green, 2.0f);
			// 게임 오버 화면
			if (!GameOverWidgetInstance && GameOverWidgetClass)
			{
				GameOverWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
				if (GameOverWidgetInstance)
				{
					GameOverWidgetInstance->AddToViewport();
				}
			}
		}
	}
}

void UCoreSystem::RewindOnDeath()   // ���� �� ����� ���ƿ��� �Լ� // BP���� ȣ��
{
	UE_LOG(LogTemp, Warning, TEXT("Rewind On Death"));
	GetWorld()->GetTimerManager().ClearTimer(RewindTimerHandle);
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("To Present!"), true, true, FLinearColor::Green, 2.0f);

	// ����� �ö�
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

	OwnerCharacter->UnEquipWeapon(); // �ֹ��� ����

	CurruntRewindCoolTime = 0.0f;

	FVector CurrentLocation = OwnerCharacter->GetActorLocation();

	FVector NewLocation = CurrentLocation - FVector(0.f, 0.f, 10000.f);
	OwnerCharacter->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RewindVFX, NewLocation, OwnerCharacter->GetActorRotation());
	UE_LOG(LogTemp, Warning, TEXT("Rewind Triggered -> Moved to: %s"), *NewLocation.ToString());

	// if core is less than 100, game is over
	if (RewindCore < 100) {
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Can't Rewind, Game Over"), true, true, FLinearColor::Green, 2.0f);
		// 게임 오버 화면
		if (!GameOverWidgetInstance && GameOverWidgetClass)
		{
			GameOverWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
			if (GameOverWidgetInstance)
			{
				GameOverWidgetInstance->AddToViewport();
			}
		}
	}
}

void UCoreSystem::CoreHeal(int value) 
{
	RewindCore += value;
	if (RewindCore >= 300) RewindCore = 300;   // Max Rewind Core �̻����� �ȿö󰡰�
}

void UCoreSystem::PauseRewind() 
{
	GetWorld()->GetTimerManager().PauseTimer(RewindTimerHandle);
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Rewind Paused"), true, true, FLinearColor::Green, 2.0f);
}

void UCoreSystem::UnPauseRewind() 
{
	GetWorld()->GetTimerManager().UnPauseTimer(RewindTimerHandle);
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Rewind Unpaused"), true, true, FLinearColor::Green, 2.0f);
}