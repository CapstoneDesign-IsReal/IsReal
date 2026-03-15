// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"
#include "Kismet/GameplayStatics.h"

AShotGun::AShotGun()
{
	Damage = 20;
	CurrentAmmo = 8;
	MaxAmmo = 8;
	FireRate = 1.0f;
	TotalAmmo = 40;

	ReloadCoolTime = 2.0f;
}

void AShotGun::ResetAmmo()
{
	CurrentAmmo = 8;
	TotalAmmo = 40;

}

void AShotGun::WeaponFire()
{
	UE_LOG(LogTemp, Warning, TEXT("ShotGun Fire!!"));
	// 들어가야 하는 것
	// 탄약 체크, 
	// 사운드 및 이펙트 출력

	// FireRate보다 빠른 연타 방지
	const float now = GetWorld()->GetTimeSeconds();
	if (now - LastFireTime < FireRate) return;
	LastFireTime = now;

	// 샷건은 다른 클래스와 달리 별도의(Scatter) 라인트레이스 함수 실행
	ShotGunFireLineTrace();
	if (!(GetWorld()->GetTimerManager().IsTimerActive(AutoFireTimer))) {
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimer, this, &AShotGun::ShotGunFireLineTrace, FireRate, true);
	}

}
void AShotGun::WeaponStopFire()
{

	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
}

void AShotGun::WeaponReload()
{

	AWeaponSystem::WeaponReload(); // 기본 리로드 기능 호출
}

void AShotGun::ShotGunFireLineTrace()
{
	// 샷건의 탄약 관리는 예외적으로 자식 클래스에서 직접 구현
	if (CurrentAmmo <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("No Ammo!"));
		return;
	}

	CurrentAmmo--;
	
	for (int i = 0; i < shotgunPellet; i++)
	{
		ScatterFireLineTrace();
	}
	UGameplayStatics::PlaySoundAtLocation(this, PumpSound, GetActorLocation());
}

void AShotGun::Interact_Implementation(AActor* Interactor)
{
	SetWeaponType(EWeaponType::EWT_Shotgun);
	//Destroy(); // 흠..Destroy가 있으면 한 몇초동안 아웃라이너에 남아있다가 사라짐
	// 이 사실을 가지고 한 몇초 플레이를 한다면 총알이 발사가 안됨. 
}

EInteractionType AShotGun::GetInteractionType_Implementation()
{
	return EInteractionType::Gun;
}