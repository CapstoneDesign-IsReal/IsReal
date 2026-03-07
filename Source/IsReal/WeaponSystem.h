// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoreEventSubsystem.h"
#include "WeaponSystem.generated.h"

class UNiagaraSystem;

UENUM(BlueprintType)
enum class EWeaponType : uint8   // 총기 종류를 고르기 위해선 setter를 사용해야 하는데 이건 총과 상호작용을 통해 설정해야함.
{
	EWT_None UMETA(DisplayName = "None"),   // 무기 없음
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_Sniper UMETA(DisplayName = "Sniper"),
	EWT_Melee UMETA(DisplayName = "Melee")
};

UCLASS()
class ISREAL_API AWeaponSystem : public AActor
{
	GENERATED_BODY()
	
protected:
	// Weapon System Variables
	// 총기 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon System")
	int Damage;
	// 현재 장전된 총알 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon System")
	int CurrentAmmo;
	// 총기 별 최대 장전 가능 총알 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon System")
	int MaxAmmo;
	// 발사 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon System")
	float FireRate;
	// 남은 총알 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon System")
	float TotalAmmo;

	// 무기 컴포넌트 // 이거 Weaponsystem으로 가져옴  25/11/16 (코드 리팩토링) 이건 삭제할지 안할지 물어봐야함
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GunMesh)
	//class UStaticMeshComponent* gunMeshComp;
	UPROPERTY(EditAnywhere, Category = "Weapon Sound")
	USoundBase* FireSound;
private:
	//  Weapon Type
	EWeaponType _weapontype;

	FVector StartVector;
	FVector FwDirection;
	;
	UStaticMeshComponent* RifleMesh;
	UStaticMeshComponent* PistolMesh;
	UStaticMeshComponent* CurrentGun; //현재 총이 메시 어떤건지 담을 변수 

	// Reload State
	bool isReloading = false;
	FTimerHandle ReloadTimerHandle;
	void WeaponReloadCooldown();

	UPROPERTY(EditAnywhere, Category = "VFX")
	TArray<UNiagaraSystem*> BloodVFXArray; 

	void PlayBloodEffect(FVector ImpactLocation, FVector ImpactNormal);
protected:
	float ReloadCoolTime;

public:
	// 델리게이트용 Subsystem
	UCoreEventSubsystem* coresubsys;
	// Reset weapon Ammo // 자식에서 오버라이드 해서 사용
	virtual void ResetAmmo();

	// Weapon Type setters and getters
	void SetWeaponType(EWeaponType NewType);
	EWeaponType GetWeaponType() const;
	// Weapon Ammo setters and getters
	void SetWeaponAmmo(int ammo);
	int GetWeaponAmmo();
	// IsReloading getter
	bool IsReloading();

	// Weapon System Functions
	virtual void WeaponFire();
	virtual void WeaponStopFire();
	virtual void WeaponReload();

	// 총 연속 쏘기 관리할 타이머 핸들러 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle AutoFireTimer;
	// 라인트레이스로 총 구현할 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireLineTrace();

public:	
	// Sets default values for this actor's properties
	AWeaponSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
