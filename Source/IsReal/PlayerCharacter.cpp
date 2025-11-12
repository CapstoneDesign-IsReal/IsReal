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
APlayerCharacter::APlayerCharacter() // �ʱ�ȭ �ϱ� 
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SocketOffset = FVector(0.f, 70.f, 50.f); // (X=�յ�, Y=�¿�, Z=���Ʒ�)
	SpringArmComp->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f)); // -15�� �����ٺ��� ����

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;




	// �� �޽� ���� ���� �ʱ�ȭ�ϱ�

	//ConstructorHelpers::FObjectFinder<UStaticMesh> rifleMesh(TEXT("/Game/Fab/Free_Gun_Packs/Meshes/AK47_Body.AK47_Body"));
	//gunMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMeshComp"));
	////GunMeshComp��� �̸����� ���ο� SkeletalMeshComponent�� �����. 
	//gunMeshComp->SetupAttachment(GetMesh());
	//// �� �� �޽ø� ��ü �޽ÿ� �ٿ���   , GetMesh()�� ĳ������ �� ���̷�Ż �޽� (��������Ʈ���� �پ�����)

	//if (rifleMesh.Succeeded())
	//{
	//	gunMeshComp->SetStaticMesh(rifleMesh.Object); //gunMeshComp�� AK47_Body �� ����
	//	// �߰�
	//	gunMeshComp->SetupAttachment(GetMesh(), TEXT("RifleSocket"));
	//	//�� �޽ø� ĳ���� ��ü�� RifleSocket�̶�� ���� ���Ϻκп� �ٿ��� 
	//}

	// �� ���̴°� ��������Ʈ�� ���̰� �׸��� �� �� ��ġ�� �������� 

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	auto pc = Cast<APlayerController>(Controller); //�÷��̾� ��Ʈ�ѷ� �������� 
	if (pc) //�����ϸ� 
	{
		pc->PlayerCameraManager->ViewPitchMin = -50.f; //@@@@@@@@@
		pc->PlayerCameraManager->ViewPitchMax = 50.f; //@@@@@@@@

		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}
	if (NormalCrossHairWidgetClass) {
		NormalCrossHairWidget = CreateWidget<UUserWidget>(GetController<APlayerController>(), NormalCrossHairWidgetClass);
		//CrossHairWidgetClass�� BP_CrosshairWidget�̸� CrossHairWidget�� ��´�. 
		// => �״ϱ� ������������Ʈ�� �÷��̾� ��������ƮŬ�������� ������ �ϴ� ������ ������ؼ�.

		if (NormalCrossHairWidget) //�׸��� �� ���� ��ü�� �����ϸ� (������ ĳ���� ��������Ʈ���� ���� ���ָ�) 
		{
			NormalCrossHairWidget->AddToViewport(); //���� ���̱� 
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
		PlayerInput->BindAction(ia_Look, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &APlayerCharacter::InputJump);

		PlayerInput->BindAction(ia_Rewind, ETriggerEvent::Started, this, &APlayerCharacter::Rewind);

		PlayerInput->BindAction(ia_ToggleClock, ETriggerEvent::Started, this, &APlayerCharacter::OnTapStarted);
		PlayerInput->BindAction(ia_ToggleClock, ETriggerEvent::Completed, this, &APlayerCharacter::OnTapCompleted);

		PlayerInput->BindAction(AimAction, ETriggerEvent::Started, this, &APlayerCharacter::DoAimStart);
		PlayerInput->BindAction(AimAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoAimEnd);

		PlayerInput->BindAction(ShootingAction, ETriggerEvent::Started, this, &APlayerCharacter::DoShootingStart);
		PlayerInput->BindAction(ShootingAction, ETriggerEvent::Completed, this, &APlayerCharacter::DoShootingEnd);

		PlayerInput->BindAction(ia_Interact, ETriggerEvent::Started, this, &APlayerCharacter::PInteract);
	}

}

void APlayerCharacter::Look(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	AddControllerYawInput(value.X);
	AddControllerPitchInput(value.Y);
}

void APlayerCharacter::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir, value.X);
	AddMovementInput(RightDir, value.Y);
}

void APlayerCharacter::InputJump(const FInputActionValue& inputValue)
{
	Jump();
}

void APlayerCharacter::Rewind(const FInputActionValue& inputValue)
{
	FVector CurrentLocation = GetActorLocation();
	if (RewindCore > 0 && !(GetWorldTimerManager().IsTimerActive(RewindTimerHandle))) {

		if (Is_Rewind == false)
		{
			RewindCoolTime = 10.0f; // 여기엔 추후 변수를 하나 둬서 아이템같은거 먹으면 체류시간 늘어나게 할 수 있음
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
		UE_LOG(LogTemp, Warning, TEXT("Rewind Triggered -> Moved to: %s"), *NewLocation.ToString());

		// ���⼭ Ŭ���� ���ο� ���� �ھ ȸ���ϴ� ��� �߰� �ؾ��ҵ�.
		//	if Ŭ���� ������
		//		��� �ڵ� 100 ȸ��
		//	if else Ŭ���� ���� ������ ��
		//		���׾��� - �����ſ� ����(50)��ŭ �ھ� ��ȯ
		//		�׾��� - �������� ���� �ھ� loss
	}
}

void APlayerCharacter::OnTapStarted(const FInputActionValue& inputValue)
{
	if (!ClockWidgetInstance && ClockWidgetClass)
	{
		ClockWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ClockWidgetClass);
	}

	if (ClockWidgetInstance && !ClockWidgetInstance->IsInViewport())
	{
		ClockWidgetInstance->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT("Clock UI Opened"));
	}
}

void APlayerCharacter::OnTapCompleted(const FInputActionValue& inputValue)
{
	if (ClockWidgetInstance && ClockWidgetInstance->IsInViewport())
	{
		ClockWidgetInstance->RemoveFromParent();
		ClockWidgetInstance = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Clock UI Closed"));
	}
}

void APlayerCharacter::DoAimStart()
{
	if (IsHasGun) {
		// ī�޶� ���� (�þ� ������)
		if (CameraComp)
		{
			CameraComp->SetFieldOfView(AimFOV);
		}

		// �������� ī�޶� �Ÿ� ���̱�
		if (SpringArmComp)
		{
			SpringArmComp->TargetArmLength = AimArmLength;
		}
		isAiming = true;

		if (AimCrossHairWidget) //�׸��� �� ���� ��ü�� �����ϸ� (������ ĳ���� ��������Ʈ���� ���� ���ָ�) 
		{
			AimCrossHairWidget->AddToViewport(); //���� ���̱� 
			NormalCrossHairWidget->RemoveFromParent(); //���� �����
		}
	}
}

void APlayerCharacter::DoAimEnd()
{
	if (IsHasGun) {
		// ī�޶� �ǵ�����
		if (CameraComp)
		{
			CameraComp->SetFieldOfView(DefaultFOV);
		}

		// �������� �Ÿ� ����
		if (SpringArmComp)
		{
			SpringArmComp->TargetArmLength = DefaultArmLength;
		}
		isAiming = false;
		if (NormalCrossHairWidget) //�׸��� �� ���� ��ü�� �����ϸ� (������ ĳ���� ��������Ʈ���� ���� ���ָ�) 
		{
			NormalCrossHairWidget->AddToViewport(); //���� ���̱� 
			AimCrossHairWidget->RemoveFromParent(); //���� �����
		}
	}
}

void APlayerCharacter::DoShootingStart()
{
	IsShooting = true;

	// �ܹ߿�: ��ư ���� ���� ��� �� �� �߻�
	//FireLineTrace();

	// 가지고 있는 무기에 따라 fire가 다르게 나감
	if (CurrentWeapon && IsHasGun) {
		switch (CurrentWeapon->GetWeaponType()) {
			case EWeaponType::EWT_Rifle:{
				if (CurrentWeapon->GetWeaponAmmo() > 0) {
					UE_LOG(LogTemp, Warning, TEXT("(PlayerCharacter-DoShootingStart) Fire!"));
					CurrentWeapon->WeaponFire();
					GetWorldTimerManager().SetTimer(AutoFireTimer, this, &APlayerCharacter::FireLineTrace, 0.1f, true); // 리팩토링 해야함
					UE_LOG(LogTemp, Warning, TEXT("(PlayerCharacter-DoShootingStart) Current Ammo : %d"), CurrentWeapon->GetWeaponAmmo());
				}
				else {
					CurrentWeapon->WeaponReload(); // 추후 수정 예정 : 아무것도 작동 안하게
				}
				break;
			}
			case EWeaponType::EWT_Pistol: {
				if (CurrentWeapon->GetWeaponAmmo() > 0) {
					UE_LOG(LogTemp, Warning, TEXT("(PlayerCharacter-DoShootingStart) Fire!"));
					CurrentWeapon->WeaponFire();
					GetWorldTimerManager().SetTimer(AutoFireTimer, this, &APlayerCharacter::FireLineTrace, 0.5f, true); // 리팩토링 해야함
					UE_LOG(LogTemp, Warning, TEXT("(PlayerCharacter-DoShootingStart) Current Ammo : %d"), CurrentWeapon->GetWeaponAmmo());
				}
				else {
					CurrentWeapon->WeaponReload(); // 추후 수정 예정 : 아무것도 작동 안하게
				}
				break;
			}
			default:
				break;
		}
	}

	// �����: ���� �ֱ�� �ڵ� �߻� ���� (0.1�ʸ���)
	//GetWorldTimerManager().SetTimer(AutoFireTimer, this, &APlayerCharacter::FireLineTrace, 0.1f, true);

	// �𸮾��� Ÿ�̸� �Ŵ����� �̿��� 0.1�ʸ��� FireLineTrace()�� �ݺ� �����Ų��.
	// �� 3�ʵ��� ������ �갡 �ڵ����� 30���� ��������ִ°��� 

	// ���⼭ true�� �ݺ� �÷����̴�. // this�� ������ ��ü(�Լ��� ���� Ŭ����)
	// 3��°�� �ֱ������� �ݺ��� �Լ� ������
	// AutoFireTimer��� �ڵ�(�ĺ���)�� ����

}

void APlayerCharacter::DoShootingEnd()
{
	IsShooting = false;

	// ��ư ���� �� �ݺ� ���߱�

	GetWorldTimerManager().ClearTimer(AutoFireTimer);

	// ���� ����(World)���� �۵� ���� Ÿ�̸� �Ŵ��� ��ü�� ������. �� ��ü�� ��� Ÿ�̸Ӹ� ������.
	// ClearTimer�� ������ Ÿ�̸ӿ� ���õ� �ݺ� ȣ���� �ߴ���.
	// ���� ������ �ĺ���(�ڵ�) Ÿ�̸Ӹ� ���ߴ°���.

}
void APlayerCharacter::FireLineTrace()
{
	if (IsHasGun) {
		FVector Start = CameraComp->GetComponentLocation();
		FVector ForwardVector = CameraComp->GetForwardVector();
		FVector End = Start + (ForwardVector * 20000.0f);

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

		FVector TargetPoint = End; // �⺻�� (���� ������ �� ����)
		if (Hit) {
			TargetPoint = HitResult.ImpactPoint;
			DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Red, false, 0.05f, 0, 1.5f);
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Yellow, false, 0.1f);

			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 0.05f, 0, 1.5f);
		}
		if (gunMeshComp) {
			FVector MuzzleLocation = gunMeshComp->GetSocketLocation(TEXT("WeaponSocket"));
			//FRotator MuzzleRotation = gunMeshComp->GetSocketRotation(TEXT("WeaponSocket"));

			// ī�޶� ������ �״�� ��� (�÷��̾ ������ ��������)
			//FVector ShootDirection = ForwardVector;
			FVector EndFromMuzzle = Start + (ForwardVector * 20000.0f);

			FHitResult MuzzleHit;
			FCollisionQueryParams MuzzleParams;
			MuzzleParams.AddIgnoredActor(this); // Params.AddIgnoredActor(this): �ڱ� �ڽ��� ����.

			bool MuzzleTraceHit = GetWorld()->LineTraceSingleByChannel(
				MuzzleHit, MuzzleLocation, EndFromMuzzle, ECC_Visibility, MuzzleParams
			);
			//ECC_Visibility : �����̴� ��ü�� ä�θ� ����

			if (MuzzleTraceHit)
			{
				DrawDebugLine(GetWorld(), MuzzleLocation, TargetPoint, FColor::Green, false, 0.05f, 0, 1.5f);
				DrawDebugPoint(GetWorld(), TargetPoint, 10.0f, FColor::Cyan, false, 0.1f);

				AActor* HitActor2 = MuzzleHit.GetActor();
				if (HitActor2)
				{
					UE_LOG(LogTemp, Warning, TEXT(" [Gun Trace] Hit Actor: %s"), *HitActor2->GetName());
					// 여기서 적 체력 처리
				}
			}
			else
			{
				DrawDebugLine(GetWorld(), MuzzleLocation, EndFromMuzzle, FColor::Green, false, 0.05f, 0, 1.5f);
			}
		}
	}
}
void APlayerCharacter::PInteract(const FInputActionValue & inputValue) {
	FVector Start = CameraComp->GetComponentLocation();
	FVector End = Start + (CameraComp->GetForwardVector() * 500.f);

	FHitResult HitResult; // ���� Ʈ���̽��� ������ ����
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
				UE_LOG(LogTemp, Warning, TEXT("gun type: %s"),
					*StaticEnum<EWeaponType>()->GetNameStringByValue((int64)Weapon->GetWeaponType()));
				CurrentWeapon = Weapon;
				break;
			}
			default:
				break;
		}
	}
}



