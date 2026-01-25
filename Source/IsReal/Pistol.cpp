// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

APistol::APistol()
{
	Damage = 10;
	CurrentAmmo = 10;
	MaxAmmo = 10;
	FireRate = 0.5f;
	TotalAmmo = 50;

	ReloadCoolTime = 1.5f;
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

void APistol::Interact_Implementation(AActor* Interactor)
{
	SetWeaponType(EWeaponType::EWT_Pistol);
	//Destroy(); // 흠..Destroy가 있으면 한 몇초동안 아웃라이너에 남아있다가 사라짐
	// 이 사실을 가지고 한 몇초 플레이를 한다면 총알이 발사가 안됨. 
}

EInteractionType APistol::GetInteractionType_Implementation()
{
	return EInteractionType::Gun;
}