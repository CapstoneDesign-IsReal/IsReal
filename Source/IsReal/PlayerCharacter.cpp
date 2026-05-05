// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Blueprint/UserWidget.h"
#include "Interactable.h"
#include "WeaponSystem.h"
#include "WeaponBox.h"
#include "CoreSystem.h"
#include "HealthComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "EnemyEventSubsystem.h"


// Sets default values
APlayerCharacter::APlayerCharacter()  
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));

	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = DefaultArmLength;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SocketOffset = FVector(0.f, 70.f, 50.f); 
	//SpringArmComp->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f)); 

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;


	// core system Component
	CoreSystemComp = CreateDefaultSubobject<UCoreSystem>(TEXT("CoreSystemComp"));

	// health system Component
	HealthSystemComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthSystemComp"));

	SkeletalMeshForEffect = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshForEffect"));
	SkeletalMeshForEffect->SetupAttachment(GetMesh()); // 일단 캐릭터 메시에 붙임

	MovementEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MovementEffect"));
	MovementEffect->SetupAttachment(SkeletalMeshForEffect);

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		pc->PlayerCameraManager->ViewPitchMin = -50.f;
		pc->PlayerCameraManager->ViewPitchMax = 50.f;

		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}
	if (NormalCrossHairWidgetClass) {
		NormalCrossHairWidget = CreateWidget<UUserWidget>(GetController<APlayerController>(), NormalCrossHairWidgetClass);


		if (NormalCrossHairWidget)
		{
			NormalCrossHairWidget->AddToViewport();
		}
	}
	if (AimCrossHairWidgetClass) {
		AimCrossHairWidget = CreateWidget<UUserWidget>(GetController<APlayerController>(), AimCrossHairWidgetClass);
	}
	if (ShotgunCrossHairWidgetClass) {
		ShotgunCrossHairWidget = CreateWidget<UUserWidget>(GetController<APlayerController>(), ShotgunCrossHairWidgetClass);
	}
	if (SniperCrossHairWidgetClass) {
		SniperCrossHairWidget = CreateWidget<UUserWidget>(GetController<APlayerController>(), SniperCrossHairWidgetClass);
	}

	if (SniperAmmoWidgetClass) {
		SniperAmmoWidget = CreateWidget<UUserWidget>(GetController<APlayerController>(), SniperAmmoWidgetClass);
	}

	WeaponSlot.SetNum(2); // 2가지 무기 슬롯 초기화

	AnimInstance = GetMesh()->GetAnimInstance();

	if (MovementEffect)
	{
		MovementEffect->Deactivate();
	}
	SkeletalMeshForEffect->SetHiddenInGame(true);

	UGameInstance* GameInst = GetWorld()->GetGameInstance();
	if (GameInst)
	{
		coresubsystem = GameInst->GetSubsystem<UCoreEventSubsystem>();

		coresubsystem->RewindDoneDelegate.AddUObject(this, &APlayerCharacter::DetachWeapon);
	}
	if (GetWorld()) {
		EnemyEventSubsystem = GetWorld()->GetSubsystem<UEnemyEventSubsystem>();
	}


}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{

		PlayerInput->BindAction(ia_Rewind, ETriggerEvent::Started, this, &APlayerCharacter::Rewind);


		PlayerInput->BindAction(ia_ToggleClock, ETriggerEvent::Started, this, &APlayerCharacter::ToggleClock);

		PlayerInput->BindAction(AimAction, ETriggerEvent::Started, this, &APlayerCharacter::DoAimStart);
		PlayerInput->BindAction(AimAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoAimEnd);

		PlayerInput->BindAction(ShootingAction, ETriggerEvent::Started, this, &APlayerCharacter::DoShootingStart);
		PlayerInput->BindAction(ShootingAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoShootingEnd);

		PlayerInput->BindAction(ia_Reload, ETriggerEvent::Started, this, &APlayerCharacter::Reload);

		PlayerInput->BindAction(ia_Interact, ETriggerEvent::Started, this, &APlayerCharacter::PInteract);

		PlayerInput->BindAction(ia_EquipPrimary, ETriggerEvent::Started, this, &APlayerCharacter::EquipPrimaryWeapon);
		PlayerInput->BindAction(ia_EquipSecondary, ETriggerEvent::Started, this, &APlayerCharacter::EquipSecondaryWeapon);

		PlayerInput->BindAction(ia_Zoom, ETriggerEvent::Triggered, this, &APlayerCharacter::Wheel);

	}

}
void APlayerCharacter::UpdateCrosshair()
{
	// 일단 다 끄기
	if (NormalCrossHairWidget) NormalCrossHairWidget->RemoveFromParent();
	if (AimCrossHairWidget) AimCrossHairWidget->RemoveFromParent();
	if (ShotgunCrossHairWidget) ShotgunCrossHairWidget->RemoveFromParent();
	if (SniperCrossHairWidget) SniperCrossHairWidget->RemoveFromParent();
	if (SniperAmmoWidget) SniperAmmoWidget->RemoveFromParent();

	// 총 없으면 Normal UI
	if (!CurrentWeapon || !IsHasGun)
	{
		if (NormalCrossHairWidget)
			NormalCrossHairWidget->AddToViewport();
		return;
	}

	switch (CurrentWeapon->GetWeaponType())
	{
	case EWeaponType::EWT_Shotgun:
		if (ShotgunCrossHairWidget)
			ShotgunCrossHairWidget->AddToViewport();
		break;

	case EWeaponType::EWT_Sniper:
		if (isAiming)
		{
			if (SniperCrossHairWidget)
				SniperCrossHairWidget->AddToViewport();
			if (SniperAmmoWidget) 
				SniperAmmoWidget->AddToViewport();
		}
		// 조준 안하면 아무것도 안 띄움
		break;

	case EWeaponType::EWT_Rifle:
	case EWeaponType::EWT_Pistol:
		if (AimCrossHairWidget)
			AimCrossHairWidget->AddToViewport();
		break;

	default:
		break;
	}
}

void APlayerCharacter::StartAfterImage()
{
	if (MovementEffect)
	{
		MovementEffect->Activate();
	}
}

void APlayerCharacter::StopAfterImage()
{
	if (MovementEffect)
	{
		MovementEffect->Deactivate();
	}
}


void APlayerCharacter::Rewind(const FInputActionValue& inputValue)
{
	if (IsLookTimer) return;
	if (IsDie) return;
	if (CoreSystemComp) 
	{
		CoreSystemComp->TryReWind();
	}
}

void APlayerCharacter::Wheel(const FInputActionValue& inputValue) {
	float Axis = inputValue.Get<float>();

	if (!isAiming)return;
	if (!CurrentWeapon||CurrentWeapon->GetWeaponType() != EWeaponType::EWT_Sniper) return;
	CurrentSniperFOV -= Axis * ZoomSpeed; //5.f만큼 확대 및 축소가 되는셈

	CurrentSniperFOV = FMath::Clamp(CurrentSniperFOV, MaxSniperFOV, MinSniperFOV);

	CameraComp->SetFieldOfView(CurrentSniperFOV);

}

void APlayerCharacter::ToggleClock(const FInputActionValue& inputValue)
{
	if (IsDie || IsDieAnim)
	{
		return;
	}
	// 몽타주 실행 중인데 그게 시계 보는거면 닫기
	if (AnimInstance && AnimInstance->Montage_IsPlaying(nullptr))
	{
		// 시계 보는 중이면 닫기
		if (IsLookTimer)
		{
			if (ToggleClockMontage)
			{
				AnimInstance->Montage_Stop(0.01f, ToggleClockMontage);
			}

			if (ClockWidgetInstance)
			{
				ClockWidgetInstance->RemoveFromParent();
			}

			SpringArmComp->TargetArmLength = DefaultArmLength;
			IsLookTimer = false;
			if (CurrentWeapon) {
			CurrentWeapon->CanShooting = true;
			}
		}

		return;
	}

	// 위젯 생성 및 준비하기

	if (!ClockWidgetInstance && ClockWidgetClass)
	{
		ClockWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ClockWidgetClass);
	}
	// 위젯 보이게 하기
	if (!IsLookTimer)
	{
		// 혹시 조준 중이면 강제 해제
		if (isAiming)
		{
			DoAimEnd();
		}
		// 열기
		if (AnimInstance && ToggleClockMontage)
		{
			if (CurrentWeapon) {
				CurrentWeapon->CanShooting = false;
			}
			AnimInstance->Montage_Play(ToggleClockMontage);
		}

		ClockWidgetInstance->AddToViewport();
		SpringArmComp->TargetArmLength = 100;
		IsLookTimer = true;
	}
}

void APlayerCharacter::DoAimStart()
{
	if (IsRolling) return;
	if (IsLookTimer) return; //타이머 보는동안 줌 안되게
	if (IsHasGun) {
		if (CameraComp&& SpringArmComp)
		{
			if (CurrentWeapon && CurrentWeapon->GetWeaponType() == EWeaponType::EWT_Sniper)
			{
				// 저격총 
				//CameraComp->SetFieldOfView(SniperFOV);
				CurrentSniperFOV = MinSniperFOV;
				CameraComp->SetFieldOfView(CurrentSniperFOV);
				SpringArmComp->TargetArmLength = SniperArmLength;
				SpringArmComp->SocketOffset = SniperSocketOffset;
				//캐릭터 메시 안보이게 하기 
				GetMesh()->SetOwnerNoSee(true);
				//총도 숨기기 (크로스헤어에 삐죽 튀어나오기 때문에)
				if (CurrentWeapon)
				{
					CurrentWeapon->SetActorHiddenInGame(true);
				}
			}
			else
			{
				// 일반 무기
				CameraComp->SetFieldOfView(AimFOV);
				SpringArmComp->TargetArmLength = AimArmLength;
				SpringArmComp->SocketOffset = FVector(0.f, 40.f, 70.f);
			}
		}
		isAiming = true;
		UpdateMoveSpeed();
		UpdateCrosshair();
	}
}

void APlayerCharacter::DoAimEnd()
{
	if (IsHasGun) {
		if (CameraComp && SpringArmComp)
		{
			CameraComp->SetFieldOfView(DefaultFOV);
			SpringArmComp->TargetArmLength = DefaultArmLength;
			SpringArmComp->SocketOffset = FVector(0.f, 70.f, 50.f);
			if (CurrentWeapon && CurrentWeapon->GetWeaponType() == EWeaponType::EWT_Sniper)
			{
				GetMesh()->SetOwnerNoSee(false);
				CurrentWeapon->SetActorHiddenInGame(false);
			}
		}
		isAiming = false;
		UpdateMoveSpeed();
		UpdateCrosshair();
	}
}

void APlayerCharacter::DoShootingStart()
{
	// 재장전 중일 때는 발사 못하게 막기
	if (!CurrentWeapon || IsDie) return;
	if (CurrentWeapon->GetIsReloading()) return;

	// 가지고 있는 무기에 따라 fire가 다르게 나감 // 근데 굳이 switch문 안써도 될거같음

	if (IsHasGun) {
		IsShooting = true;
		UpdateMoveSpeed();
		CurrentWeapon->WeaponFire();
	}
}

void APlayerCharacter::DoShootingEnd()
{
	if (CurrentWeapon) {
		IsShooting = false;
		UpdateMoveSpeed();
		CurrentWeapon->WeaponStopFire();
	}

}

void APlayerCharacter::Reload(const FInputActionValue& inputValue)
{	
	if (IsShooting) return;
	if (IsRolling) return;
	if (CurrentWeapon && IsHasGun) {
		//CurrentWeapon->WeaponReload();
		if (CurrentWeapon->GetIsReloading() == true) return;
	}
}

void APlayerCharacter::EquipPrimaryWeapon(const struct FInputActionValue& inputValue)
{
	EquipWeapon(EWeaponSlot::Primary);
}
void APlayerCharacter::EquipSecondaryWeapon(const struct FInputActionValue& inputValue)
{
	EquipWeapon(EWeaponSlot::Secondary);
}

void APlayerCharacter::EquipWeapon(EWeaponSlot NewSlot)
{
	if (IsRolling)
		return;
	if (WeaponSlot[(int)NewSlot]) {
		DoAimEnd();
		DoShootingEnd();
		CurrentWeapon = WeaponSlot[(int)NewSlot];
		IsHasGun = true;
		UE_LOG(LogTemp, Warning, TEXT("Equipped Weapon Slot: %d"), (int)NewSlot);
		AttachWeapon();
		UpdateCrosshair();
	}
}

void APlayerCharacter::UnEquipWeapon() 
{
	if(IsShooting || isAiming) 
	{
		DoShootingEnd(); // 발사 중이면 발사 종료
		DoAimEnd(); // 조준 중이면 조준 종료
	}
	// 애니메이션 스테이트 전환에서 총안든 상태로 전환
	IsRifleEquipped = false;
	IsPistolEquipped = false;
	IsSniperEquipped = false;
	IsShotgunEquipped = false;

	IsHasGun = false;
	if (CurrentWeapon) CurrentWeapon = nullptr;
	WeaponSlot[(int)EWeaponSlot::Primary] = nullptr;
	WeaponSlot[(int)EWeaponSlot::Secondary] = nullptr;

	UE_LOG(LogTemp, Warning, TEXT("Unequipped Weapon"));
}

void APlayerCharacter::PlayerDie() {
	//플레이어 사망 처리 함수에 다음과 같은 코드 추가
	EnemyEventSubsystem->PlayerDieNotifyToEnemy();
	IsDie = true;
	IsDieAnim = true;
	//AnimInstance->Montage_Play(DieMontage);
	DoShootingEnd(); // 죽을 때 발사 멈추기
	DoAimEnd(); // 죽을 때 조준 멈추기

	// 시계 켜진 상태로 죽으면 강제로 끄기
	if (IsLookTimer)
	{
		if (AnimInstance && ToggleClockMontage)
		{
			AnimInstance->Montage_Stop(0.1f, ToggleClockMontage);
		}

		if (ClockWidgetInstance)
		{
			ClockWidgetInstance->RemoveFromParent();
		}

		IsLookTimer = false;
	}

	StopAfterImage();
	//DetachWeapon();
	SpringArmComp->TargetArmLength = 400.f;
}

void APlayerCharacter::PlayerHit(float Damage) 
{
	HealthSystemComp->hit(Damage);
}

void APlayerCharacter::Playerknockback() // delete
{
	// Rolling 중이면 Return; 데미지는 입고 모션은 안풀리고 
	HealthSystemComp->SetIsInvincible(true);

	GetWorld()->GetTimerManager().SetTimer(KnockbackTimer, this, &APlayerCharacter::KnockbackEnd, 1.0f, false);

	//if (isAiming)
	//{
	//	//DoAimEnd();
	//}
	if (IsLookTimer) { //시계를 보고 있었으면 시계를 끄기 
		if (AnimInstance && ToggleClockMontage)
		{
			AnimInstance->Montage_Stop(0.1f, ToggleClockMontage);
		}

		if (ClockWidgetInstance)
		{
			ClockWidgetInstance->RemoveFromParent();
		}

		SpringArmComp->TargetArmLength = DefaultArmLength;
		IsLookTimer = false;
	}

	KnockbackMotion();

	//if (AnimInstance && HitMontage)
	//{
	//	AnimInstance->Montage_Play(HitMontage);
	//}
}

//void APlayerCharacter::KnockbackMotion() //얘는 블프에서 실행될 함수라 만들면 두번 실행된다고 오류가난다.
//{
//
//}

void APlayerCharacter::KnockbackEnd()
{
	HealthSystemComp->SetIsInvincible(false);
	GetWorld()->GetTimerManager().ClearTimer(KnockbackTimer);
}

void APlayerCharacter::PInteract(const FInputActionValue& inputValue) {
	if (IsShooting) DoShootingEnd(); // 발사 중일 때, 상호작용 누르면 발사 멈추기

	FVector Start = CameraComp->GetComponentLocation();
	FVector End = Start + (CameraComp->GetForwardVector() * 500.f);

	FHitResult HitResult; 
	FCollisionQueryParams param;
	param.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, param);
	if (!bHit) return;

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
		return;

	if (HitActor->Implements<UInteractable>()) {
		EInteractionType InteractionType = IInteractable::Execute_GetInteractionType(HitActor);

		switch (InteractionType) {
		case EInteractionType::Door: {
			IInteractable::Execute_Interact(HitActor, this);
			break;
		}
		case EInteractionType::Box: {
			if (!CanInteractBox)return;
			if (IsRolling)return;
			IInteractable::Execute_Interact(HitActor, this);
			AWeaponBox* weaponbox = Cast<AWeaponBox>(HitActor);
			AWeaponSystem* Weapon = weaponbox->SpawnWeapon();
			Weapon->SetOwner(this);
			UE_LOG(LogTemp, Warning, TEXT("gun type: %s"),
				*StaticEnum<EWeaponType>()->GetNameStringByValue((int64)Weapon->GetWeaponType()));

			Type = Weapon->GetWeaponType();
			EWeaponSlot slot = EWeaponSlot::Primary;
			switch (Type)
			{
			case EWeaponType::EWT_Rifle:
			case EWeaponType::EWT_Shotgun:
			case EWeaponType::EWT_Sniper:
			{
				slot = EWeaponSlot::Primary;
				break;
			}
			case EWeaponType::EWT_Pistol:
			{
				slot = EWeaponSlot::Secondary;
				break;
			}
			}
			DoAimEnd();
			// @@@@@@@@@@@@@기존 같은 슬롯의 무기는 무조건 Detach하고 없애고 
			AWeaponSystem* OldWeapon = WeaponSlot[(int)slot];
			if (OldWeapon) {
				OldWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				OldWeapon->Destroy();
			}
			// @@@@@@@@@@@@그 다음에 새무기 넣기
			WeaponSlot[(int)slot] = Weapon;
			
			CurrentWeapon = Weapon;
			IsHasGun = true; // 이건 추후에 BP에서 설정안하게 하면 추가하면됨
			// 그리고 맨위에 weaponsocket같은거 attach여기서 하면될거같은데
			AttachWeapon();
			UpdateCrosshair();
			break;
		}
		case EInteractionType::Monitor: {
			IInteractable::Execute_Interact(HitActor, this);
			break;
		}
		case EInteractionType::Card:
			IInteractable::Execute_Interact(HitActor, this);
			break;
		default:
			break;
		}
	}

}
void APlayerCharacter::AttachWeapon()
{
	if (!CurrentWeapon) return;

	for (int i = 0; i < WeaponSlot.Num(); i++) //CurrentWeapon이 아닌 얘들만 숨겨줌
	{
		AWeaponSystem* Weapon = WeaponSlot[i];
		if (Weapon && Weapon != CurrentWeapon)
		{
			Weapon->SetActorHiddenInGame(true);
		}
	}

	// 그래도 혹시 모르니 현재 무기만 보이게 한번더 설정한다.
	CurrentWeapon->SetActorHiddenInGame(false);


	FName SocketName = NAME_None;
	

	switch (CurrentWeapon->GetWeaponType()) { //여기서는 Type을 써도 되는게 interact한 weapon이 Weapon 변수기 때문에 Weapon 변수의 get weapon type을 해서 가져온게 Type이기 때문에 
	case EWeaponType::EWT_Pistol:
		SocketName = TEXT("Pistol");
		break;
	case EWeaponType::EWT_Rifle:
		SocketName = TEXT("Rifle");
		break;
	case EWeaponType::EWT_Sniper:
		SocketName = TEXT("Sniper_R");
		break;
	case EWeaponType::EWT_Shotgun:
		SocketName = TEXT("Shotgun");
		break;
	}
	SetWeaponEquipped();

	if (!SocketName.IsNone())
	{
		CurrentWeapon->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketName
		);
	}
}

// 모든 무기 제거
void APlayerCharacter::DetachWeapon() 
{
	for (int i = 0; i < WeaponSlot.Num(); i++)
	{
		AWeaponSystem* Weapon = WeaponSlot[i];

		if (Weapon)
		{
			Weapon->Destroy(); // 레벨에서 삭제
		}
	}
	UnEquipWeapon();
	UpdateCrosshair();
}


void APlayerCharacter::SetWeaponEquipped() {
	IsRifleEquipped = false;
	IsPistolEquipped = false;
	IsSniperEquipped = false;
	IsShotgunEquipped = false;

	if (!CurrentWeapon)return;
	switch (CurrentWeapon->GetWeaponType()) {
	case EWeaponType::EWT_Pistol:
		PlayGetSecondaryMontage();
		IsPistolEquipped = true;
		break;
	case EWeaponType::EWT_Rifle:
		PlayGetPrimaryMontage();
		IsRifleEquipped = true;
		break;
	case EWeaponType::EWT_Sniper:
		PlayGetPrimaryMontage();
		IsSniperEquipped = true;
		break;
	case EWeaponType::EWT_Shotgun:
		PlayGetPrimaryMontage();
		IsShotgunEquipped = true;
		break;
	default:
		break;
	}
}

void APlayerCharacter::PlayGetPrimaryMontage()
{
	if (AnimInstance && GetRifleMontage)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->CanShooting = false;
			CanInteractBox = false;
		}

		AnimInstance->Montage_Play(GetRifleMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &APlayerCharacter::MontageEnded);

		AnimInstance->Montage_SetEndDelegate(EndDelegate, GetRifleMontage);
	}
}

void APlayerCharacter::PlayGetSecondaryMontage()
{
	if (AnimInstance && GetPistolMontage)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->CanShooting = false;
			CanInteractBox = false;
		}

		AnimInstance->Montage_Play(GetPistolMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &APlayerCharacter::MontageEnded);

		AnimInstance->Montage_SetEndDelegate(EndDelegate, GetPistolMontage);
	}
}

void APlayerCharacter::MontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if ((Montage == GetRifleMontage || Montage == GetPistolMontage) && CurrentWeapon)
	{
		CurrentWeapon->CanShooting = true;
		CanInteractBox = true;
	}
}


AWeaponSystem* APlayerCharacter::GetPrimaryWeapon() const
{
	if (WeaponSlot.Num() > 0)
	{
		return WeaponSlot[(int)EWeaponSlot::Primary];
	}
	return nullptr;
}

AWeaponSystem* APlayerCharacter::GetSecondaryWeapon() const
{
	if (WeaponSlot.Num() > 1)
	{
		return WeaponSlot[(int)EWeaponSlot::Secondary];
	}
	return nullptr;
}

EWeaponType APlayerCharacter::GetPrimaryWeaponType() const
{
	AWeaponSystem* Weapon = GetPrimaryWeapon();
	if (Weapon)
	{
		return Weapon->GetWeaponType();
	}

	return EWeaponType::EWT_None; // 네 enum에 None 있어야 함
}

EWeaponType APlayerCharacter::GetSecondaryWeaponType() const
{
	AWeaponSystem* Weapon = GetSecondaryWeapon();
	if (Weapon)
	{
		return Weapon->GetWeaponType();
	}

	return EWeaponType::EWT_None;
}

void APlayerCharacter::UpdateMoveSpeed()
{
	if (isAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
	}
}


