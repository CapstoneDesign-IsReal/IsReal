// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

APistol::APistol()
{
	Damage = 10;
	CurrentAmmo = 10;
	MaxAmmo = 10;
	FireRate = 0.5f;
	TotalAmmo = 50;

	// 반동 변수
	PitchRecoilAmount = 0.6f;
	YawRecoilAmount = 0.15f;
}

void APistol::BeginPlay()
{
	Super::BeginPlay();
	// Reset Weapon Type when weapon is spawned
	SetWeaponType(EWeaponType::EWT_Pistol);
}

void APistol::ResetAmmo()
{
	CurrentAmmo = 10;
	TotalAmmo = 50;

}

void APistol::WeaponFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Pistol Fire!!"));
	// 들어가야 하는 것
	// 탄약 체크, 
	// 사운드 및 이펙트 출력
	
	// FireRate보다 빠른 연타 방지
	const float now = GetWorld()->GetTimeSeconds();
	if (now - LastFireTime < FireRate) return; 
	LastFireTime = now;

	FireLineTrace();

	if (!(GetWorld()->GetTimerManager().IsTimerActive(AutoFireTimer))) {
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimer, this, &APistol::FireLineTrace, FireRate, true);
	}

}
void APistol::WeaponStopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
}
void APistol::WeaponReload()
{
	AWeaponSystem::WeaponReload(); // 기본 리로드 기능 호출
}
