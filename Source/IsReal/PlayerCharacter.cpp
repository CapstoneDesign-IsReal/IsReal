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


	Rifle1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RifleMesh"));
	Rifle1->SetupAttachment(GetMesh()); //여기까지하면 블루프린트에 생김 

	Pistol1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PistolMesh"));
	Pistol1->SetupAttachment(GetMesh()); //여기까지 하면 블루프린트에 생김 

	Rifle1->SetupAttachment(GetMesh(), TEXT("Rifle")); //캐릭터 스켈레톤 매시의 라이플이라는 소켓에 장착
	Pistol1->SetupAttachment(GetMesh(), TEXT("Pistol"));// 캐릭터 스켈레톤 매시의 피스톨이라는 소켓에 장착


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
	}

}


void APlayerCharacter::Rewind(const FInputActionValue& inputValue)
{
	FVector CurrentLocation = GetActorLocation();
	if (RewindCore > 0 && !(GetWorldTimerManager().IsTimerActive(RewindTimerHandle))) {

		if (Is_Rewind == false)
		{
			RewindCoolTime = 60.0f; // 여기엔 추후 변수를 하나 둬서 아이템같은거 먹으면 체류시간 늘어나게 할 수 있음
			GetWorldTimerManager().SetTimer(RewindTimerHandle, this, &APlayerCharacter::RewindCooldown, 1.0f, true);

			// 과거로 갈때
			Is_Rewind = true;
			RewindCore -= 100;
			if (RewindVFX)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RewindVFX, CurrentLocation, GetActorRotation()); // 위치 이동 전 VFX
			}

			FVector NewLocation = CurrentLocation + FVector(0.f, 0.f, 10000.f);
			SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
			UE_LOG(LogTemp, Warning, TEXT("Rewind Triggered -> Moved to: %s"), *NewLocation.ToString());

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RewindVFX, NewLocation, GetActorRotation());  // 위치 이동 후 VFX
		}
	}
	else {
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Can't Rewind"), true, true, FLinearColor::Green, 2.0f);
	}
}

void APlayerCharacter::RewindCooldown()
{
	RewindCoolTime--;

	UKismetSystemLibrary::PrintString(						// 쿨다운 보여주는 텍스트(지워도됨)
		GetWorld(),
		FString::Printf(TEXT("CoolDown.. : %.1f"), RewindCoolTime)
		, true, true, FLinearColor::Green, 2.0f);

	if (RewindCoolTime <= 0) {
		GetWorldTimerManager().ClearTimer(RewindTimerHandle);
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("To Present!"), true, true, FLinearColor::Green, 2.0f);

		// 현재로 올때
		Is_Rewind = false;

		FVector CurrentLocation = GetActorLocation();

		FVector NewLocation = CurrentLocation - FVector(0.f, 0.f, 10000.f);
		SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RewindVFX, NewLocation, GetActorRotation());
		UE_LOG(LogTemp, Warning, TEXT("Rewind Triggered -> Moved to: %s"), *NewLocation.ToString());

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
	if (!CurrentWeapon) return;
	//if (CurrentWeapon->IsReloading()) return;
	// 가지고 있는 무기에 따라 fire가 다르게 나감
	if (IsHasGun) {
		IsShooting = true;

		switch (CurrentWeapon->GetWeaponType()) {
		case EWeaponType::EWT_Rifle: {
			if (CurrentWeapon->GetWeaponAmmo() > 0) {
				CurrentWeapon->WeaponFire();
				UE_LOG(LogTemp, Warning, TEXT("(PlayerCharacter-DoShootingStart) Current Ammo : %d"), CurrentWeapon->GetWeaponAmmo());
			}
			else {
				CurrentWeapon->WeaponStopFire();
			}
			break;
		}
		case EWeaponType::EWT_Pistol: {
			if (CurrentWeapon->GetWeaponAmmo() > 0) {
				CurrentWeapon->WeaponFire();
				UE_LOG(LogTemp, Warning, TEXT("(PlayerCharacter-DoShootingStart) Current Ammo : %d"), CurrentWeapon->GetWeaponAmmo());
			}
			else {
				CurrentWeapon->WeaponStopFire();
			}
			break;
		}
		default:
			break;
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
{	// 추후에 isReloading 같은 변수를 통해 재장전할때 발사 못하게 막아야함
	if (CurrentWeapon && IsHasGun) {
		CurrentWeapon->WeaponReload();
	}
}

// getters and setters for PlayerHP
float APlayerCharacter::GetPlayerHP() { return PlayerHP; }

void APlayerCharacter::SetPlayerHP(float HP) { 
	PlayerHP = HP;
	UE_LOG(LogTemp, Warning, TEXT("PlayerHP = %f"), PlayerHP);
	if (PlayerHP <= 0) {
		PlayerDie();
	}
}

void APlayerCharacter::PlayerDie() {
	IsDie = true;
}

void APlayerCharacter::PInteract(const FInputActionValue& inputValue) {
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
			CurrentWeapon = Weapon;
			IsHasGun = true; // 이건 추후에 BP에서 설정안하게 하면 추가하면됨
			// 그리고 맨위에 weaponsocket같은거 attach여기서 하면될거같은데
			break;
		}
		case EInteractionType::Monitor: {
			IInteractable::Execute_Interact(HitActor, this);
			break;
		}
		default:
			break;
		}
	}
}


