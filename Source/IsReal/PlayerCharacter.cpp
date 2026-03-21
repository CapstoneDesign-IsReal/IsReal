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
#include "CoreSystem.h"
#include "HealthComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerCharacter::APlayerCharacter()  
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SocketOffset = FVector(0.f, 70.f, 50.f); 
	SpringArmComp->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f)); 

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
	// 위젯 생성 (없으면 생성)
	if (!ClockWidgetInstance && ClockWidgetClass)
	{
		ClockWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ClockWidgetClass);
	}

	// 토글: 열려 있으면 → 닫고 / 안 열렸으면 → 열기
	if (ClockWidgetInstance)
	{
		if (IsLookTimer)
		{
			// 닫기
			ClockWidgetInstance->RemoveFromParent();
			SpringArmComp->TargetArmLength = 400;
			UE_LOG(LogTemp, Warning, TEXT("Clock UI Closed"));
		}
		else
		{
			// 열기
			ClockWidgetInstance->AddToViewport();
			SpringArmComp->TargetArmLength = 250;
			UE_LOG(LogTemp, Warning, TEXT("Clock UI Opened"));
		}

		// 상태 반전
		IsLookTimer = !IsLookTimer;
	}
}


void APlayerCharacter::DoAimStart()
{
	if (IsHasGun) {
		
		if (CameraComp)
		{
			CameraComp->SetFieldOfView(AimFOV);
		}

		
		if (SpringArmComp)
		{
			SpringArmComp->TargetArmLength = AimArmLength;
		}
		isAiming = true;

		if (AimCrossHairWidget) 
		{
			AimCrossHairWidget->AddToViewport(); 
			NormalCrossHairWidget->RemoveFromParent(); 
		}
	}
}

void APlayerCharacter::DoAimEnd()
{
	if (IsHasGun) {
		
		if (CameraComp)
		{
			CameraComp->SetFieldOfView(DefaultFOV);
		}

		
		if (SpringArmComp)
		{
			SpringArmComp->TargetArmLength = DefaultArmLength;
		}
		isAiming = false;
		if (NormalCrossHairWidget) 
		{
			NormalCrossHairWidget->AddToViewport(); 
			AimCrossHairWidget->RemoveFromParent(); 
		}
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

		CurrentWeapon->WeaponStopFire();
	}

}

void APlayerCharacter::Reload(const FInputActionValue& inputValue)
{	
	if (IsShooting) return;
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
	DoShootingEnd(); // 죽을 때 발사 멈추기
	DoAimEnd(); // 죽을 때 조준 멈추기
}

void APlayerCharacter::PlayerHit(float Damage) 
{
	HealthSystemComp->hit(Damage);
}

void APlayerCharacter::Playerknockback()
{
	HealthSystemComp->SetIsInvincible(true);

	GetWorld()->GetTimerManager().SetTimer(KnockbackTimer, this, &APlayerCharacter::KnockbackEnd, 1.0f, false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(HitMontage);

	AnimInstance->OnMontageEnded.AddDynamic(
		this,
		&APlayerCharacter::OnHitMontageEnded
	);
}

void APlayerCharacter::OnHitMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HitMontage)
	{
		//HealthSystemComp->SetIsInvincible(false);
	}
}

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


