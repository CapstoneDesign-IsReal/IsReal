// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

APistol::APistol()
{
	Damage = 10;
	CurrentAmmo = 10;
	MaxAmmo = 10;
}

void APistol::WeaponFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Pistol Fire!!"));
	// 들어가야 하는 것
	// 탄약 체크, 탄 수 감소
	CurrentAmmo--;
	// 사운드 및 이펙트 출력
	// 라인 트레이스 (playerCharacter에 있는 FireLineTrace()를 리팩토링 해야할듯?)
	// 피격 처리
	// 연사속도 제어

}

void APistol::WeaponReload()
{
	CurrentAmmo = MaxAmmo;
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