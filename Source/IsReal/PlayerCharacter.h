// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

class UNiagaraSystem;
class AWeaponSystem;

#include "PlayerCharacter.generated.h"

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
	TSubclassOf<UUserWidget> ClockWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* ClockWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* RewindVFX;




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






	void Rewind(const struct FInputActionValue& inputValue); 

	void PInteract(const struct FInputActionValue& inputValue);

	void ToggleClock(const FInputActionValue& Value); 

	void Reload(const struct FInputActionValue& inputValue); // R 눌렀을 때 실행됨

	virtual void DoAimStart();

	virtual void DoAimEnd();

	virtual void DoShootingStart();

	virtual void DoShootingEnd();




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	bool IsHasGun = false; 




	// weapon system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeaponSystem* CurrentWeapon;




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultFOV = 90.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimFOV = 65.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultArmLength = 400.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimArmLength = 200.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	bool isAiming = false; 




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle AutoFireTimer;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsShooting = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRange = 3000.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSetting)
	bool Is_Rewind = false;

	
	UPROPERTY(EditAnywhere, BluePrintReadWrite) 
	bool IsLookTimer = false;

	//Player Current HP
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	float PlayerHP = 100.0;

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	bool IsDie = false;

	// getters and setters for Player HP
public:
	float GetPlayerHP();
	void SetPlayerHP(float HP);


private:
	// Rewind Core
	int RewindCore = 300;
	// Rewind Cooldown
	float RewindCoolTime;
	FTimerHandle RewindTimerHandle;
	// Rewind Cooldown function
	void RewindCooldown();

	void PlayerDie();

};