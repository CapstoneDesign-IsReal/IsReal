#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Door.generated.h"

UCLASS()
class ISREAL_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoor();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual EInteractionType GetInteractionType_Implementation() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// 문 상태
	bool _IsOpen = false;
	bool bMoving = false; // 문이 움직이는 중인지

	// 문 컴포넌트
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LeftDoor;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RightDoor;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorButton;

	// 위치 저장
	FVector LeftClosedPos;
	FVector RightClosedPos;
	FVector LeftOpenPos;
	FVector RightOpenPos;

	// 문 이동 속도
	UPROPERTY(EditAnywhere, Category = "Door")
	float MoveSpeed = 2.0f; // 1~5 정도 조절 가능
};