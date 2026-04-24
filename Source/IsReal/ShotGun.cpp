// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"
#include "WeaponSystem.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"


AShotGun::AShotGun()
{
	Damage = 20;
	CurrentAmmo = 8;
	MaxAmmo = 8;
	FireRate = 1.0f;
	TotalAmmo = 40;

	// 반동 변수
	PitchRecoilAmount = 1.0f;
	YawRecoilAmount = 0.2f;
}

void AShotGun::BeginPlay()
{
	Super::BeginPlay();
	// Reset Weapon Type when weapon is spawned
	SetWeaponType(EWeaponType::EWT_Shotgun);
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
		UGameplayStatics::PlaySoundAtLocation(this, NoAmmoSound, GetActorLocation());
		return;
	}

	CurrentAmmo--;
	
	for (int i = 0; i < shotgunPellet; i++)
	{
		ScatterFireLineTrace();
	}
	UGameplayStatics::PlaySoundAtLocation(this, PumpSound, GetActorLocation());

	APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwner());
	// 총구 화염 이펙트 출력
	if (GetWeaponType() == EWeaponType::EWT_Shotgun) ShotGunMesh = PC->GetRifleMesh(); // 임시로 라이플 메시 사용
	else ShotGunMesh = nullptr;

	FVector SGMuzzleLoc = ShotGunMesh->GetSocketLocation(TEXT("WeaponSocket"));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFireVFX, SGMuzzleLoc, GetActorRotation());

	// 총기 반동 적용
	ApplyRecoil();
}
