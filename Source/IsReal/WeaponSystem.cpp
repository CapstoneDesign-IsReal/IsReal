// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystem.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

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

	UGameInstance* GameInst = GetWorld()->GetGameInstance();
	if (GameInst)
	{
		coresubsys = GameInst->GetSubsystem<UCoreEventSubsystem>();

		coresubsys->RewindDoneDelegate.AddUObject(this, &AWeaponSystem::ResetAmmo);
	}
}

void AWeaponSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (coresubsys)
	{
		coresubsys->RewindDoneDelegate.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AWeaponSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponSystem::ResetAmmo()
{
	// Implement in child classes
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
	if (CurrentAmmo == MaxAmmo) return; // 이미 총알이 가득 찬 경우

	if (TotalAmmo <= 0) 
	{	// 추후 사운드 이펙트 추가 - 총알 없다는 경고
		UE_LOG(LogTemp, Warning, TEXT("No More Ammo to Reload!"));
		return;
	}

	if (!(GetWorld()->GetTimerManager().IsTimerActive(ReloadTimerHandle)))
	{
		if (isReloading == false)
		{
			isReloading = true;
			// 리로드 쿨타임 타이머 시작
			GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AWeaponSystem::WeaponReloadCooldown, ReloadCoolTime, false);

			// 필요한 총알 수 계산
			int NeededAmmo = MaxAmmo - CurrentAmmo;
			if (NeededAmmo <= 0) return; // 이미 총알이 가득 찬 경우

			int LoadedAmmo = FMath::Min(NeededAmmo, TotalAmmo); // 실제로 장전할 총알 수

			CurrentAmmo += LoadedAmmo; // 현재 총알 수 증가
			TotalAmmo -= LoadedAmmo;
		}
	}	
}
void AWeaponSystem::WeaponReloadCooldown()
{
	UE_LOG(LogTemp, Warning, TEXT("Reload Complete!"));
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	isReloading = false;
}

void AWeaponSystem::ApplyRecoil()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController) {
		APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
		if (Player)
		{
			float FinalPitchAmount = PitchRecoilAmount;
			float FinalYawAmount = YawRecoilAmount;

			if (Player->GetIsAiming())		// 조준 상태에서는 반동 감소
			{
				FinalPitchAmount *= 0.3f;
				FinalYawAmount *= 0.3f;
			}

			float PitchRecoil = FMath::RandRange(FinalPitchAmount * 0.8f, FinalPitchAmount * 1.2f);
			float YawRecoil = FMath::RandRange(-FinalYawAmount, FinalYawAmount);

			PlayerController->AddPitchInput(-PitchRecoil); // Pitch는 위로 올라가므로 음수 입력
			PlayerController->AddYawInput(YawRecoil);   // Yaw는 좌우로 흔들리므로 양수/음수 입력

		}
	}
}

// Weapon Type setters and getters
void AWeaponSystem::SetWeaponType(EWeaponType NewType) { _weapontype = NewType; }

EWeaponType AWeaponSystem::GetWeaponType() const { return _weapontype; }

// Weapon Ammor setters and gettters
void AWeaponSystem::SetWeaponAmmo(int ammo) { CurrentAmmo = ammo; }

int AWeaponSystem::GetWeaponAmmo() { return CurrentAmmo; }

bool AWeaponSystem::IsReloading(){ return isReloading; }

void AWeaponSystem::FireLineTrace()
{
	if (!CanShooting) return;
	if (CurrentAmmo <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("No Ammo!"));
		return;
	}
	// 한발 쏠 때마다 ammo -1 
	CurrentAmmo--;
	// 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());

	// 총기 반동 적용
	ApplyRecoil();

	APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwner());
	if (PC) {
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
	FVector TargetNormal = FVector::ZeroVector;
	if (Hit) {
		TargetPoint = HitResult.ImpactPoint;
		TargetNormal = HitResult.ImpactNormal;

		AActor* HitActor = HitResult.GetActor();

	}
	else
	{
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.05f, 0, 1.5f);
	}

	CurrentGun = nullptr;

	switch (_weapontype)
	{
	case EWeaponType::EWT_Rifle:
		CurrentGun = PC->GetRifleMesh();
		break;
	case EWeaponType::EWT_Pistol:
		CurrentGun = PC->GetPistolMesh();
		break;
	case EWeaponType::EWT_Sniper:
		CurrentGun = PC->GetRifleMesh(); // 일단 임시로 라이플 메쉬 사용
		break;
	default:
		CurrentGun = nullptr;
		break;
	}

	FVector MuzzleLocation = CurrentGun->GetSocketLocation(TEXT("WeaponSocket"));
	//FRotator MuzzleRotation = gunMeshComp->GetSocketRotation(TEXT("WeaponSocket"));

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
		//DrawDebugLine(GetWorld(), MuzzleLocation, TargetPoint, FColor::Red, false, 0.05f, 0, 1.5f);


		AActor* HitActor2 = MuzzleHit.GetActor();
		if (HitActor2)
		{
			// 여기서 적 체력 처리
			AEnemy* Enemy = Cast<AEnemy>(HitActor2);
			if (Enemy)
			{
				//DrawDebugPoint(GetWorld(), TargetPoint, 10.0f, FColor::Blue, false, 0.1f); // 적군을 맞추면 파란 점 찍힘
				UE_LOG(LogTemp, Warning, TEXT(" [Gun Trace] Hit Actor: %s"), *Enemy->GetName());
				Enemy->Hit(Damage, MuzzleHit.BoneName);    // 총기 별 데미지 주기
				PlayBloodEffect(TargetPoint, TargetNormal); // 피격 이펙트 재생
			}
		}
	}
	else
	{
		//DrawDebugLine(GetWorld(), MuzzleLocation, EndFromMuzzle, FColor::Red, false, 0.05f, 0, 1.5f);
	}

	// 총구 화염 이펙트 재생 - MuzzleLocation에서 VFX 실행
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFireVFX, MuzzleLocation, GetActorRotation());
	
	// Bullet Trail VFX 재생
	UNiagaraComponent* TrailNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		BulletTrailVFX,
		MuzzleLocation
	);

	TrailNiagaraComp->SetVectorParameter(TEXT("start"), MuzzleLocation);
	TrailNiagaraComp->SetVectorParameter(TEXT("end"), (EndFromMuzzle - MuzzleLocation));
}

// 샷건 라인트레이스 - 샷건의 탄약은 예외적으로 자식 클래스(AShotGun)에서 관리합니다. 
//					  why? Pump Sound를 총이 발사될 때 마다 재생해야 하기 때문입니다.
void AWeaponSystem::ScatterFireLineTrace()
{
	if (!CanShooting) return;
	// 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());

	APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwner());
	if (PC) {
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
	FVector TargetNormal = FVector::ZeroVector;
	if (Hit) {
		TargetPoint = HitResult.ImpactPoint;
		TargetNormal = HitResult.ImpactNormal;

		AActor* HitActor = HitResult.GetActor();

	}
	else
	{
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.05f, 0, 1.5f);
	}

	CurrentGun = nullptr;

	if (_weapontype == EWeaponType::EWT_Shotgun) CurrentGun = PC->GetRifleMesh();  // 임시로 라이플 메쉬 사용중
	else CurrentGun = nullptr;

	FVector MuzzleLocation = CurrentGun->GetSocketLocation(TEXT("WeaponSocket"));
	//FRotator MuzzleRotation = gunMeshComp->GetSocketRotation(TEXT("WeaponSocket"));

	float Range = 20000.0f;
	float DistanceToSphere = 1000.0f;
	float SphereRadius = 80.0f;
	FVector Dir = (TargetPoint - MuzzleLocation).GetSafeNormal(); // GetSafeNormal(): To make normalized vector
	// 샷건의 탄환이 퍼지는 범위 
	FVector SphereCenter = MuzzleLocation + (Dir * DistanceToSphere);
	FVector RandVec = FMath::VRand() * FMath::FRandRange(0.0f, SphereRadius);
	FVector EndLoc = SphereCenter + RandVec;
	// 탄환이 퍼지는 범위에서 랜덤한 위치로 라인트레이스 끝벡터 설정
	FVector EndFromMuzzle = (EndLoc - MuzzleLocation) * Range;

	FHitResult MuzzleHit;
	FCollisionQueryParams MuzzleParams;
	MuzzleParams.AddIgnoredActor(PC);
	MuzzleParams.AddIgnoredActor(this); // Params.AddIgnoredActor(this): �ڱ� �ڽ��� ����.

	bool MuzzleTraceHit = GetWorld()->LineTraceSingleByChannel(
		MuzzleHit, MuzzleLocation, EndFromMuzzle, ECC_Visibility, MuzzleParams
	);
	//ECC_Visibility : �����̴� ��ü�� ä�θ� ����

	//DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, false, 1.0f); // 샷건 탄환이 퍼지는 범위 시각화

	if (MuzzleTraceHit)
	{
		//DrawDebugLine(GetWorld(), MuzzleLocation, TargetPoint, FColor::Red, false, 0.05f, 0, 1.5f);


		AActor* HitActor2 = MuzzleHit.GetActor();
		if (HitActor2)
		{
			// 여기서 적 체력 처리
			AEnemy* Enemy = Cast<AEnemy>(HitActor2);
			if (Enemy)
			{
				//DrawDebugLine(GetWorld(), MuzzleLocation, TargetPoint, FColor::Blue, false, 1.0f); // 적군을 맞추면 파란 점 찍힘
				UE_LOG(LogTemp, Warning, TEXT(" [Gun Trace] Hit Actor: %s"), *Enemy->GetName());
				Enemy->Hit(Damage, MuzzleHit.BoneName);    // 총기 별 데미지 주기
				PlayBloodEffect(TargetPoint, TargetNormal); // 피격 이펙트 재생
			}
		}
	}
	else
	{
		//DrawDebugLine(GetWorld(), MuzzleLocation, EndFromMuzzle, FColor::Red, false, 1.0f);
	}

	// Bullet Trail VFX 재생
	UNiagaraComponent* TrailNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		BulletTrailVFX,
		MuzzleLocation
	);

	TrailNiagaraComp->SetVectorParameter(TEXT("start"), MuzzleLocation);
	TrailNiagaraComp->SetVectorParameter(TEXT("end"), (EndFromMuzzle - MuzzleLocation));
}

void AWeaponSystem::PlayBloodEffect(FVector impactpoint, FVector impactnormal) 
{
	if (BloodVFXArray.Num() > 0)
	{	
		int32 RandomIndex = FMath::RandRange(0, BloodVFXArray.Num() - 1);
		UNiagaraSystem* RandBloodEffect = BloodVFXArray[RandomIndex];

		float OffsetDistance = 10.0f; // 이펙트가 표면에서 약간 떨어지도록 하는 거리
		FVector SurfacePoint = impactpoint + (impactnormal * OffsetDistance);

		FRotator Rotation = impactnormal.Rotation();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RandBloodEffect, SurfacePoint, Rotation);

	}
}