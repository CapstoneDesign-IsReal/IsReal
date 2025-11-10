// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSystem.generated.h"

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
	// Weapon System Functions
	UFUNCTION(BlueprintCallable, Category = "Weapon System")
	virtual void WeaponFire();
	UFUNCTION(BlueprintCallable, Category = "Weapon System")
	virtual void WeaponReload();
	// Weapon System Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon System")
	int Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon System")
	int CurrentAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon System")
	int MaxAmmo;

private:
	//  Weapon Type
	EWeaponType _weapontype;


public:
	// Weapon Type setters and getters
	void SetWeaponType(EWeaponType NewType);
	EWeaponType GetWeaponType() const;

public:	
	// Sets default values for this actor's properties
	AWeaponSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
