#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"

class ACard;
#include "Door.generated.h"

UCLASS()
class ISREAL_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADoor();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual EInteractionType GetInteractionType_Implementation() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// 문 상태
	bool _IsOpen = false;
	bool bMoving = false;

	// 문 컴포넌트
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LeftDoor;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RightDoor;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorButton;

	

	UPROPERTY(EditAnywhere, Category = "Door")
	ACard* LinkedCard; // 카드 연결

	// 위치 저장
	FVector LeftClosedPos;
	FVector RightClosedPos;
	FVector LeftOpenPos;
	FVector RightOpenPos;

	// 문 이동 속도
	UPROPERTY(EditAnywhere, Category = "Door")
	float MoveSpeed = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Door_Display")
	UChildActorComponent* LockedDisplayComp;

	UPROPERTY(VisibleAnywhere, Category = "Door_Display")
	UChildActorComponent* UnlockedDisplayComp;
	bool bIsDisplayUnlocked = false;
	bool IsCardPresent() const;
};