// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperRifle.h"

ASniperRifle::ASniperRifle()
{
	Damage = 150;
	CurrentAmmo = 5;
	MaxAmmo = 5;
	FireRate = 1.5f;
	TotalAmmo = 25;

	// 반동 변수
	PitchRecoilAmount = 1.3f;
	YawRecoilAmount = 0.05f;

	ReloadCoolTime = 1.5f;
}

void ASniperRifle::ResetAmmo()
{
	CurrentAmmo = 5;
	TotalAmmo = 25;
}

void ASniperRifle::WeaponFire()
{
	UE_LOG(LogTemp, Warning, TEXT("SinperRifle Fire!!"));
	// 들어가야 하는 것
	// 탄약 체크, 
	// 사운드 및 이펙트 출력

	// FireRate보다 빠른 연타 방지
	const float now = GetWorld()->GetTimeSeconds();
	if (now - LastFireTime < FireRate) return;
	LastFireTime = now;

	FireLineTrace();

	if (!(GetWorld()->GetTimerManager().IsTimerActive(AutoFireTimer))) {
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimer, this, &ASniperRifle::FireLineTrace, FireRate, true);
	}

}
void ASniperRifle::WeaponStopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
}
void ASniperRifle::WeaponReload()
{
	AWeaponSystem::WeaponReload(); // 기본 리로드 기능 호출
}

void ASniperRifle::Interact_Implementation(AActor* Interactor)
{
	SetWeaponType(EWeaponType::EWT_Sniper);
	//Destroy(); // 흠..Destroy가 있으면 한 몇초동안 아웃라이너에 남아있다가 사라짐
	// 이 사실을 가지고 한 몇초 플레이를 한다면 총알이 발사가 안됨. 
}

EInteractionType ASniperRifle::GetInteractionType_Implementation()
{
	return EInteractionType::Gun;
}