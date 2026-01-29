// Fill out your copyright notice in the Description page of Project Settings.


#include "Rifle.h"

ARifle::ARifle()
{
	Damage = 15;
	CurrentAmmo = 30;
	MaxAmmo = 30;
	FireRate = 0.1f;
	TotalAmmo = 120;

	ReloadCoolTime = 2.0f;
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


void ARifle::Interact_Implementation(AActor* Interactor)
{
	SetWeaponType(EWeaponType::EWT_Rifle);
	//Destroy(); // 흠..Destroy가 있으면 한 몇초동안 아웃라이너에 남아있다가 사라짐
	// 이 사실을 가지고 한 몇초 플레이를 한다면 총알이 발사가 안됨. 
}

EInteractionType ARifle::GetInteractionType_Implementation()
{
	return EInteractionType::Gun;
}