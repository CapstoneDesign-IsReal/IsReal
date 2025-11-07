#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoor"));
	LeftDoor->SetupAttachment(RootComponent);

	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoor"));
	RightDoor->SetupAttachment(RootComponent);

	DoorButton = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorButton"));
	DoorButton->SetupAttachment(RootComponent);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	// 초기 위치 저장
	LeftClosedPos = LeftDoor->GetRelativeLocation();
	RightClosedPos = RightDoor->GetRelativeLocation();

	// 열림 위치 계산
	LeftOpenPos = LeftClosedPos + FVector(0.f, -200.f, 0.f);
	RightOpenPos = RightClosedPos + FVector(0.f, 200.f, 0.f);
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bMoving) return;

	// 목표 위치 선택
	FVector TargetLeft = _IsOpen ? LeftOpenPos : LeftClosedPos;
	FVector TargetRight = _IsOpen ? RightOpenPos : RightClosedPos;

	// 부드럽게 이동
	FVector NewLeft = FMath::VInterpTo(LeftDoor->GetRelativeLocation(), TargetLeft, DeltaTime, MoveSpeed);
	FVector NewRight = FMath::VInterpTo(RightDoor->GetRelativeLocation(), TargetRight, DeltaTime, MoveSpeed);

	LeftDoor->SetRelativeLocation(NewLeft);
	RightDoor->SetRelativeLocation(NewRight);

	// 거의 도착하면 이동 종료
	if (FVector::Dist(NewLeft, TargetLeft) < 1.f)
	{
		bMoving = false;
		LeftDoor->SetRelativeLocation(TargetLeft);
		RightDoor->SetRelativeLocation(TargetRight);
	}
}

void ADoor::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Door Interacted"));

	// 상태 토글 및 이동 시작
	_IsOpen = !_IsOpen;
	bMoving = true;
}

EInteractionType ADoor::GetInteractionType_Implementation()
{
	return EInteractionType::Door;
}