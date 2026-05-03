// Fill out your copyright notice in the Description page of Project Settings.


#include "Rifle.h"

ARifle::ARifle()
{
	Damage = 15;
	CurrentAmmo = 30;
	MaxAmmo = 30;
	FireRate = 0.1f;
	TotalAmmo = 120;

	// 반동 변수
	PitchRecoilAmount = 0.35f;
	YawRecoilAmount = 0.08f;
}

void ARifle::BeginPlay()
{
	Super::BeginPlay();
	// Reset Weapon Type when weapon is spawned
	SetWeaponType(EWeaponType::EWT_Rifle);
}

void ARifle::ResetAmmo()
{
	CurrentAmmo = 30;
	TotalAmmo = 120;

}

void ARifle::WeaponFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Rifle Fire!!"));
	// 들어가야 하는 것
	// 탄약 체크, 
	// 사운드 및 이펙트 출력

	// FireRate보다 빠른 연타 방지
	const float now = GetWorld()->GetTimeSeconds();
	if (now - LastFireTime < FireRate) return;
	LastFireTime = now;

	FireLineTrace();
	if (!(GetWorld()->GetTimerManager().IsTimerActive(AutoFireTimer))) {
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimer, this, &ARifle::FireLineTrace, FireRate, true);
	}
	
}
void ARifle::WeaponStopFire()
{

	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
}

void ARifle::WeaponReload()
{
	
	AWeaponSystem::WeaponReload(); // 기본 리로드 기능 호출
}

