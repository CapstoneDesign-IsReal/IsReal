// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponSystem.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

class UNiagaraSystem;
class AWeaponSystem;
class UCoreSystem;
class UHealthComponent;
class AWeaponBox;
class UNiagaraComponent;

#include "PlayerCharacter.generated.h"

enum class EWeaponSlot : uint8
{
	Primary = 0,
	Secondary = 1
};


UCLASS()
class ISREAL_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	APlayerCharacter();

public:
	// 카메라 Weaponsystem에서 접근해야 해서 public으로 변경 22/11/19
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GunMesh)
	class UStaticMeshComponent* gunMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh) // To use WeaponSystem
		class UStaticMeshComponent* Rifle1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GunMesh) 
		class UStaticMeshComponent* Pistol1;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UStaticMeshComponent* GetRifleMesh() const { return Rifle1; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UStaticMeshComponent* GetPistolMesh() const { return Pistol1; }



protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> AimCrossHairWidgetClass; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* AimCrossHairWidget; 



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> NormalCrossHairWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* NormalCrossHairWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ShotgunCrossHairWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* ShotgunCrossHairWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> SniperCrossHairWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* SniperCrossHairWidget;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ClockWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* ClockWidgetInstance;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		class UInputMappingContext* imc_TPS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		class UInputAction* ia_Rewind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		class UInputAction* ia_ToggleClock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") // R (재장전)
		class UInputAction* ia_Reload;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Interact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		class UInputAction* ShootingAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") // 1번키 - 주무기
		class UInputAction* ia_EquipPrimary;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") // 2번키 - 보조무기
		class UInputAction* ia_EquipSecondary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") // Shift 구르기
		class UInputAction* ia_Roll;



	//void Roll(const struct FInputActionValue& inputValue);

	void Rewind(const struct FInputActionValue& inputValue); 

	void PInteract(const struct FInputActionValue& inputValue);

	void ToggleClock(const FInputActionValue& Value); 

	void Reload(const struct FInputActionValue& inputValue); // R 눌렀을 때 실행됨

	virtual void DoAimStart();

	UFUNCTION(BlueprintCallable)
	virtual void DoAimEnd();

	virtual void DoShootingStart();

	virtual void DoShootingEnd();
	
	// 주무기, 보조무기 장착 인풋 함수
	void EquipPrimaryWeapon(const struct FInputActionValue& inputValue);
	void EquipSecondaryWeapon(const struct FInputActionValue& inputValue);




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	bool IsHasGun = false; 




	// weapon system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeaponSystem* CurrentWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TArray<AWeaponSystem*> WeaponSlot;

	void EquipWeapon(EWeaponSlot NewSlot);

	// core system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Core");
	TObjectPtr<UCoreSystem> CoreSystemComp;

	// health system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health");
	TObjectPtr<UHealthComponent> HealthSystemComp;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultFOV = 90.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimFOV = 65.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultArmLength = 150.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimArmLength = 100.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	bool isAiming = false; 




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle AutoFireTimer;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle KnockbackTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsShooting = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRange = 3000.f;
	
	UPROPERTY(EditAnywhere, BluePrintReadWrite) 
	bool IsLookTimer = false;

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	bool IsDie = false;

	UPROPERTY(EditAnywhere, BluePrintReadWrite)\
	bool IsDieAnim = false; //애니메이션을 위한 die 변수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType Type;

	bool isCombat = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool IsRolling;

	void UpdateCrosshair();

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	bool IsRifleEquipped = false; //애니메이션을 스테이트 전환을 위한 변수
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	bool IsPistolEquipped = false; //애니메이션을 스테이트 전환을 위한 변수
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	bool IsSniperEquipped = false; //애니메이션을 스테이트 전환을 위한 변수
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	bool IsShotgunEquipped = false; //애니메이션을 스테이트 전환을 위한 변수

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	bool CanInteractBox = true; //무기를 interact할 때 계속 눌러서 equip하는 것을 방지하는 변수 처음에는 true여야 한다.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraComponent* MovementEffect; //잔상 나이아가라 컴포넌트

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	USkeletalMeshComponent* SkeletalMeshForEffect; //잔상 나이아가라를 붙일 스켈레탈 메시 (왜냐면 기본 우리 캐릭터 메시로 하면 스파클이 이상하게 생겨서 하나 만들어서 거기에 붙인다.)

public:
	void UnEquipWeapon();
	bool GetIsDie() { return IsDie; }
	// 전투 상태 getters and setters
	bool GetIsCombat() { return isCombat; }
	void SetIsCombat(bool combat) { isCombat = combat; }
	bool GetIsAiming() { return isAiming; }

	// 슬롯에 들어있는 무기 자체 반환
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	AWeaponSystem* GetPrimaryWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	AWeaponSystem* GetSecondaryWeapon() const;

	// 슬롯 무기 타입만 반환
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponType GetPrimaryWeaponType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponType GetSecondaryWeaponType() const;


	void PlayerDie();

	void PlayerHit(float damage);
	void Playerknockback();

	void UpdateMoveSpeed();

	void StartAfterImage(); // 잔상 보이게 하는 함수
	void StopAfterImage(); // 잔상 안보이게 하는 함수

	
	// 애님몽타주
	UPROPERTY(EditAnywhere)
	UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ToggleClockMontage;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	UPROPERTY(EditAnywhere)
	UAnimMontage* GetRifleMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* GetPistolMontage;

	UFUNCTION(BlueprintImplementableEvent)
	void KnockbackMotion();
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// 플레이어의 애님인스턴스 자체를 가져오는 변수
	UAnimInstance* AnimInstance;

private:
	void KnockbackEnd();
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void AttachWeapon(); //CurrentWeapon만 Attach하게 하는 함수
	void PlayGetPrimaryMontage(); // 주무기를 interact하는 함수
	void PlayGetSecondaryMontage(); // 보조무기를 interact 함수
	void SetWeaponEquipped(); // Equipped 불변수를 셋팅하는 함수
	void MontageEnded(UAnimMontage* Montage, bool bInterruted);
	//void MontageBlendOut(UAnimMontage* Montage, bool bInterruted); //애니메이션이 중간에 끊길 때 사용할 함수.

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
};