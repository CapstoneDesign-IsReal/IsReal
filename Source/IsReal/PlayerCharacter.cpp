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

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#include "Rifle.h"
#include "ShotGun.h"
#include "SniperRifle.h"
#include "Pistol.h"
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

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


	Rifle1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RifleMesh")); //여기까지하면 블루프린트에 생김 
	Rifle1->SetupAttachment(GetMesh());
	Pistol1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PistolMesh")); //여기까지하면 블루프린트에 생김 
	Pistol1->SetupAttachment(GetMesh());


	// core system Component
	CoreSystemComp = CreateDefaultSubobject<UCoreSystem>(TEXT("CoreSystemComp"));

	// health system Component
	HealthSystemComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthSystemComp"));

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

	WeaponSlot.SetNum(2); // 2가지 무기 슬롯 초기화

	if (Rifle1)
	{
		Rifle1->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("Rifle")
		);
	}

	if (Pistol1)
	{
		Pistol1->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("Pistol")
		);
	}
	AnimInstance = GetMesh()->GetAnimInstance();
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

	}

}
void APlayerCharacter::UpdateCrosshair()
{
	// 일단 다 끄기
	if (NormalCrossHairWidget) NormalCrossHairWidget->RemoveFromParent();
	if (AimCrossHairWidget) AimCrossHairWidget->RemoveFromParent();
	if (ShotgunCrossHairWidget) ShotgunCrossHairWidget->RemoveFromParent();
	if (SniperCrossHairWidget) SniperCrossHairWidget->RemoveFromParent();

	// 총 없으면 Normal UI
	if (!CurrentWeapon || !IsHasGun)
	{
		if (NormalCrossHairWidget)
			NormalCrossHairWidget->AddToViewport();
		return;
	}

	EWeaponType Type = CurrentWeapon->GetWeaponType();

	switch (Type)
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



void APlayerCharacter::Rewind(const FInputActionValue& inputValue)
{
	if (IsDie) return;
	if (CoreSystemComp) 
	{
		CoreSystemComp->TryReWind();
	}
}

void APlayerCharacter::ToggleClock(const FInputActionValue& inputValue)
{
	// 몽타주 실행 중
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
		}

		return;
	}

	// 몽타주 없으면 그냥 토글

	if (!ClockWidgetInstance && ClockWidgetClass)
	{
		ClockWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ClockWidgetClass);
	}

	if (!ClockWidgetInstance) return;

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
			AnimInstance->Montage_Play(ToggleClockMontage);
		}

		ClockWidgetInstance->AddToViewport();
		SpringArmComp->TargetArmLength = 100;
		IsLookTimer = true;
	}
}

void APlayerCharacter::DoAimStart()
{
	if (IsLookTimer) return; //타이머 보는동안 줌 안되게
	if (IsHasGun) {
		if (CameraComp&& SpringArmComp)
		{
			if (CurrentWeapon->GetWeaponType() == EWeaponType::EWT_Sniper)
			{
				// 저격총 
				CameraComp->SetFieldOfView(20.f);
				SpringArmComp->TargetArmLength = 50.f;
				SpringArmComp->SocketOffset = FVector(0.f, 20.f, 70.f);
				//캐릭터 메시 안보이게 하기 
				GetMesh()->SetOwnerNoSee(true);
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
			GetMesh()->SetOwnerNoSee(false);
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
	if (CurrentWeapon->IsReloading()) return;

	// 가지고 있는 무기에 따라 fire가 다르게 나감 // 근데 굳이 switch문 안써도 될거같음

	if (IsHasGun) {
		IsShooting = true;
		UpdateMoveSpeed();
		if (CurrentWeapon->GetWeaponAmmo() > 0) 
		{
			CurrentWeapon->WeaponFire();
		}
		else
		{
			CurrentWeapon->WeaponStopFire();
		}
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
		CurrentWeapon->WeaponReload();
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
	if (WeaponSlot[(int)NewSlot]) {
		CurrentWeapon = WeaponSlot[(int)NewSlot];
		IsHasGun = true;
		UE_LOG(LogTemp, Warning, TEXT("Equipped Weapon Slot: %d"), (int)NewSlot);
	}
	UpdateCrosshair();
}

void APlayerCharacter::UnEquipWeapon()
{
	if(IsShooting || isAiming) 
	{
		DoShootingEnd(); // 발사 중이면 발사 종료
		DoAimEnd(); // 조준 중이면 조준 종료
	}
	if (WeaponSlot[(int)EWeaponSlot::Secondary] == nullptr) // 보조무기가 없다면
	{
		IsHasGun = false;
		CurrentWeapon = nullptr;
		WeaponSlot[(int)EWeaponSlot::Primary] = nullptr;
	}
	else												   // 보조무기가 있으면 보조로 교체
	{
		CurrentWeapon = WeaponSlot[(int)EWeaponSlot::Secondary];
		WeaponSlot[(int)EWeaponSlot::Primary] = nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("Unequipped Weapon"));
}

void APlayerCharacter::PlayerDie() {
	IsDie = true;
	IsDieAnim = true;
	//AnimInstance->Montage_Play(DieMontage);
	DoShootingEnd(); // 죽을 때 발사 멈추기
	DoAimEnd(); // 죽을 때 조준 멈추기
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

	if (isAiming)
	{
		DoAimEnd();
	}
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

//void APlayerCharacter::KnockbackMotion()
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
		case EInteractionType::Gun: {
			IInteractable::Execute_Interact(HitActor, this);
			AWeaponSystem* Weapon = Cast<AWeaponSystem>(HitActor);
			Weapon->SetOwner(this);
			UE_LOG(LogTemp, Warning, TEXT("gun type: %s"),
				*StaticEnum<EWeaponType>()->GetNameStringByValue((int64)Weapon->GetWeaponType()));

			type = Weapon->GetWeaponType();
			EWeaponSlot slot = EWeaponSlot::Primary;
			switch (type) 
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
			WeaponSlot[(int)slot] = Weapon;
			CurrentWeapon = Weapon;
			IsHasGun = true; // 이건 추후에 BP에서 설정안하게 하면 추가하면됨
			// 그리고 맨위에 weaponsocket같은거 attach여기서 하면될거같은데
			UpdateCrosshair();
			break;
		}
		case EInteractionType::Box: {
			IInteractable::Execute_Interact(HitActor, this);
			AWeaponBox* weaponbox = Cast<AWeaponBox>(HitActor);
			AWeaponSystem* Weapon = weaponbox->SpawnWeapon();
			Weapon->SetOwner(this);
			UE_LOG(LogTemp, Warning, TEXT("gun type: %s"),
				*StaticEnum<EWeaponType>()->GetNameStringByValue((int64)Weapon->GetWeaponType()));

			type = Weapon->GetWeaponType();
			EWeaponSlot slot = EWeaponSlot::Primary;
			switch (type)
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
			WeaponSlot[(int)slot] = Weapon;
			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			//기존 무기 제거
			if (CurrentWeapon) 
			{
				CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			}
			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			CurrentWeapon = Weapon;
			IsHasGun = true; // 이건 추후에 BP에서 설정안하게 하면 추가하면됨
			// 그리고 맨위에 weaponsocket같은거 attach여기서 하면될거같은데
			AttachWeapon(Weapon);
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
void APlayerCharacter::AttachWeapon(AWeaponSystem* Weapon)
{
	if (!Weapon) return;

	FName SocketName = NAME_None;

	if (Weapon->IsA(ARifle::StaticClass()))
	{
		SocketName = TEXT("Rifle");
		FirstGetPrimary();
	}
	else if (Weapon->IsA(AShotGun::StaticClass()))
	{
		SocketName = TEXT("Shotgun");
		FirstGetPrimary();
	}
	else if (Weapon->IsA(ASniperRifle::StaticClass()))
	{
		SocketName = TEXT("Sniper");
		FirstGetPrimary();
	}
	else if (Weapon->IsA(APistol::StaticClass()))
	{
		SocketName = TEXT("Pistol");
		FirstGetSecondary();
	}
	else
	{
		SocketName = TEXT("Default");
	}
	SetWeaponEquipped(Weapon);

	if (!SocketName.IsNone())
	{
		Weapon->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketName
		);
	}
}

void APlayerCharacter::SetWeaponEquipped(AWeaponSystem* Weapon) {
	IsRifleEquipped = false;
	IsPistolEquipped = false;
	IsSniperEquipped = false;
	IsShotgunEquipped = false;

	if (!Weapon)return;
	if (Weapon->IsA(ARifle::StaticClass())) {
		IsRifleEquipped = true;
	}
	else if (Weapon->IsA(AShotGun::StaticClass())) {
		IsShotgunEquipped = true;
	}
	else if (Weapon->IsA(ASniperRifle::StaticClass())) {
		IsSniperEquipped = true;
	}
	else if (Weapon->IsA(APistol::StaticClass())) {
		IsPistolEquipped = true;
	}
}

void APlayerCharacter::FirstGetPrimary()
{
	if (AnimInstance && GetRifleMontage)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->CanShooting = false;
		}

		AnimInstance->Montage_Play(GetRifleMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &APlayerCharacter::MontageEnded);

		AnimInstance->Montage_SetEndDelegate(EndDelegate, GetRifleMontage);
	}
}

void APlayerCharacter::FirstGetSecondary()
{
	if (AnimInstance && GetPistolMontage)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->CanShooting = false;
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


