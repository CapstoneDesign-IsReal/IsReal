// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystem.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Enemy.h"

// Sets default values
AWeaponSystem::AWeaponSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void AWeaponSystem::BeginPlay()
{
	Super::BeginPlay();
	SetWeaponType(EWeaponType::EWT_None); // Initial Type
}

// Called every frame
void AWeaponSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponSystem::WeaponFire()
{
	// Implement in child classes
}
void AWeaponSystem::WeaponStopFire()
{
	// Implement in child classes
}
void AWeaponSystem::WeaponReload()
{
	// Implement in child classes
}

// Weapon Type setters and getters
void AWeaponSystem::SetWeaponType(EWeaponType NewType) { _weapontype = NewType; }

EWeaponType AWeaponSystem::GetWeaponType() const { return _weapontype; }

// Weapon Ammor setters and gettters
void AWeaponSystem::SetWeaponAmmo(int ammo) { CurrentAmmo = ammo; }

int AWeaponSystem::GetWeaponAmmo() { return CurrentAmmo; }

void AWeaponSystem::FireLineTrace()
{
	if(CurrentAmmo <= 0){
		UE_LOG(LogTemp, Warning, TEXT("No Ammo!"));
		return;
	}
	// 한발 쏠 때마다 ammo -1 
	CurrentAmmo--;

	APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwner());
	if (PC){
		StartVector = PC->CameraComp->GetComponentLocation();
		FwDirection = PC->CameraComp->GetForwardVector();
	}
	if (!PC)
	{
		return;
	}

	FVector Start = StartVector;
	FVector ForwardVector = FwDirection;
	FVector End = StartVector + (ForwardVector * 20000.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PC);

	bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	FVector TargetPoint = End; // �⺻�� (���� ������ �� ����)
	if (Hit) {
		TargetPoint = HitResult.ImpactPoint;
		//DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Red, false, 0.05f, 0, 1.5f);
		//DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Blue, false, 0.1f);

		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
		}
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.05f, 0, 1.5f);
	}


	RifleMesh = PC->GetRifleMesh();
	PistolMesh = PC->GetPistolMesh();

	CurrentGun = nullptr;

	if (_weapontype == EWeaponType::EWT_Rifle)
	{
		CurrentGun = RifleMesh;
	}
	else if (_weapontype == EWeaponType::EWT_Pistol)
	{
		CurrentGun = PistolMesh;
	}
	if (!CurrentGun) return;

	FVector MuzzleLocation = CurrentGun->GetSocketLocation(TEXT("WeaponSokect"));
	//FRotator MuzzleRotation = gunMeshComp->GetSocketRotation(TEXT("WeaponSocket"));

	// ī�޶� ������ �״�� ��� (�÷��̾ ������ ��������)
	//FVector ShootDirection = ForwardVector;

	float Range = 20000.0f;
	FVector Dir = (TargetPoint - MuzzleLocation).GetSafeNormal(); // GetSafeNormal(): To make normalized vector
	FVector EndFromMuzzle = MuzzleLocation + (Dir * Range);

	FHitResult MuzzleHit;
	FCollisionQueryParams MuzzleParams;
	MuzzleParams.AddIgnoredActor(PC);
	MuzzleParams.AddIgnoredActor(this); // Params.AddIgnoredActor(this): �ڱ� �ڽ��� ����.

	bool MuzzleTraceHit = GetWorld()->LineTraceSingleByChannel(
		MuzzleHit, MuzzleLocation, EndFromMuzzle, ECC_Visibility, MuzzleParams
	);
	//ECC_Visibility : �����̴� ��ü�� ä�θ� ����

	if (MuzzleTraceHit)
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, TargetPoint, FColor::Red, false, 0.05f, 0, 1.5f);
		DrawDebugPoint(GetWorld(), TargetPoint, 10.0f, FColor::Blue, false, 0.1f);

		AActor* HitActor2 = MuzzleHit.GetActor();
		if (HitActor2)
		{
			//UE_LOG(LogTemp, Warning, TEXT(" [Gun Trace] Hit Actor: %s"), *HitActor2->GetName());
			// 여기서 적 체력 처리
			AEnemy* Enemy = Cast<AEnemy>(HitActor2);
			if (Enemy)
			{
				UE_LOG(LogTemp, Warning, TEXT(" [Gun Trace] Hit Actor: %s"), *Enemy->GetName());
				Enemy->Hit(Damage);    // 총기 별 데미지 주기
			}
		}
	}
	else
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, EndFromMuzzle, FColor::Red, false, 0.05f, 0, 1.5f);
	}
}
