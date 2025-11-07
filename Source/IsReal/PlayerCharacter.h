// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

class UNiagaraSystem;

#include "PlayerCharacter.generated.h"

UCLASS()
class ISREAL_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// 엔진이 자동으로 호출
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 컴포넌트들

	// 스프링암
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;
	// 카메라
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;


	// 무기 컴포넌트 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GunMesh)
	class UStaticMeshComponent* gunMeshComp;


	// 총 쏠 때 크로스헤어 위젯 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> AimCrossHairWidgetClass; //위젯 블루프린트 클래스를 저장할 객체변수고 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* AimCrossHairWidget; //실제로 화면에 표시될 위젯 인스턴스(실체)를 저장할 객체변수 


	// 평상시 크로스 헤어 위젯 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> NormalCrossHairWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* NormalCrossHairWidget;


	// 시계 UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ClockWidgetClass;
	// 시계 UI 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* ClockWidgetInstance;
	// 되감기 VFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* RewindVFX;



	// 인풋 매핑 관련 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")// 인풋매핑컨텍스트
		class UInputMappingContext* imc_TPS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")// wasd (이동)
		class UInputAction* ia_Move;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")// 스페이스바 (점프)
		class UInputAction* ia_Jump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")// 마우스 (시야)
		class UInputAction* ia_Look;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")// T (시간 이동하기)
		class UInputAction* ia_Rewind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")// Tab키 (시간위젯 나오기)
		class UInputAction* ia_ToggleClock;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Interact;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")// 오른쪽 마우스 (조준)
		class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")// 왼쪽 마우스 (총쏘기)
		class UInputAction* ShootingAction;



	// 인풋 매핑에 의해 실행 될 함수 
	void Move(const struct FInputActionValue& inputValue);

	void InputJump(const struct FInputActionValue& inputValue);

	void Look(const struct FInputActionValue& inputValue);

	void Rewind(const struct FInputActionValue& inputValue); //T 눌렀을 때 실행됨 

	void PInteract(const struct FInputActionValue& inputValue);

	void OnTapStarted(const FInputActionValue& Value); // Tab 눌렀을 때 실행됨
	void OnTapCompleted(const FInputActionValue& Value); // Tab 뗐을 때 실행됨



	virtual void DoAimStart();// 오른쪽 마우스 눌렀을 때 실행됨

	virtual void DoAimEnd();// 오른쪽 뗐을 때 실행됨

	virtual void DoShootingStart();// 왼쪽 마우스 눌렀을 때 실행됨

	virtual void DoShootingEnd();// 왼쪽 마우스 뗐을 때 실행됨


	// 나중에 처리해야함 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	bool IsHasGun = false; // 총을 들고 있는지.



	// 조준 관련 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultFOV = 90.f; //원래 시야각

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimFOV = 65.f; // 조준 시야각 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultArmLength = 300.f; //원래 카메라와의 거리

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimArmLength = 200.f; // 조준시 카메라와의 거리 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	bool isAiming = false; // 조준을 하고 있는지 



	// 마우스 왼쪽버튼으로 총 쏘기 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle AutoFireTimer; // 총 연속 쏘기 관리할 타이머 핸들러 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsShooting = false; //총 쏘고 있는지

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRange = 3000.f; // 총 나가는 거리 

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireLineTrace(); // 라인트레이스로 총 구현할 함수 



	//시간 바뀔 때 관련 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSetting)
	bool Is_Rewind = false;

private:
	// Rewind Core
	int RewindCore = 300;
	// Rewind Cooldown
	float RewindCoolTime;
	FTimerHandle RewindTimerHandle;
	// Rewind Cooldown function
	void RewindCooldown();

};