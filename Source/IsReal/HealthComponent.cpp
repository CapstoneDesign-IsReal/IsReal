// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<APlayerCharacter>(GetOwner());

	// RewindDone 델리게이트에 바인딩
	UGameInstance* GameInst = GetWorld()->GetGameInstance();
	if (GameInst)
	{
		coresubsys = GameInst->GetSubsystem<UCoreEventSubsystem>();

		coresubsys->RewindDoneDelegate.AddUObject(this, &UHealthComponent::ResetHP);
	}
}
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

// getters and setters for PlayerHP
float UHealthComponent::GetPlayerHP() { return PlayerHP; }


void UHealthComponent::hit(float damage) 
{
	if (GetIsInvincible()) return;
	PlayerHP -= damage;
	player->Playerknockback();
	if (PlayerHP <= 0) {
		player->PlayerDie();
	}

	GetWorld()->GetTimerManager().ClearTimer(LastHitTimeHandle);
	GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);

	player->SetIsCombat(true);     // 공격을 받았을 때, 전투 상태로 전환

	// 공격을 받은 후 일정 시간이 지나면 전투 상태에서 벗어나도록
	GetWorld()->GetTimerManager().SetTimer(LastHitTimeHandle, this, &UHealthComponent::ExitCombat, 5.0f, false);
}

// 공격을 받지않고 가만히 있을 때, 체력이 회복되는 함수
void UHealthComponent::heal() 
{
	if (player->GetIsDie()) return;
	if (player->GetIsCombat()) return;

	if (PlayerHP < 100) {
		PlayerHP += 1.0f; // 체력 회복 속도 조절
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle); // 체력이 최대치에 도달하면 타이머 정지
	}
}

void UHealthComponent::ExitCombat()
{
	player->SetIsCombat(false); // 전투 상태에서 벗어남
	GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &UHealthComponent::heal, 0.1f, true);
	
}

void UHealthComponent::ResetHP()
{
	PlayerHP = 100.0f;
	// 추후 player->ResetPlayer() 같은 함수로 플레이어 상태(죽음 상태) 초기화하는 것으로 구현해야 할 듯
}