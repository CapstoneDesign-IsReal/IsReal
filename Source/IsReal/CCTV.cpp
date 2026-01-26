// CCTV.cpp
#include "CCTV.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"

ACCTV::ACCTV()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    CCTVMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CCTVMesh"));
    CCTVMesh->SetupAttachment(Root);

    CCTVView = CreateDefaultSubobject<UCameraComponent>(TEXT("CCTVView"));
    CCTVView->SetupAttachment(Root);

    RotationSpeed = 60.f; // 카메라 회전 속도
}

void ACCTV::BeginPlay()
{
    Super::BeginPlay();
    PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void ACCTV::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsActive || !PlayerController) return;

    // 스페이스바: CCTV 종료
    if (PlayerController->WasInputKeyJustPressed(EKeys::SpaceBar))
    {
        bIsActive = false;
        PlayerController->SetViewTargetWithBlend(PlayerController->GetPawn(), 0.0f);
        return;
    }

    // 방향키로 회전
    FRotator NewRot = CCTVView->GetRelativeRotation();

    if (PlayerController->IsInputKeyDown(EKeys::Down))    NewRot.Pitch = FMath::Clamp(NewRot.Pitch - RotationSpeed * DeltaTime, -80.f, 80.f);
    if (PlayerController->IsInputKeyDown(EKeys::Up))  NewRot.Pitch = FMath::Clamp(NewRot.Pitch + RotationSpeed * DeltaTime, -80.f, 80.f);
    if (PlayerController->IsInputKeyDown(EKeys::Left))  NewRot.Yaw -= RotationSpeed * DeltaTime;
    if (PlayerController->IsInputKeyDown(EKeys::Right)) NewRot.Yaw += RotationSpeed * DeltaTime;

    CCTVView->SetRelativeRotation(NewRot);
}

void ACCTV::Interact_Implementation(AActor* Interactor)
{
    if (!PlayerController) return;

    if (!bIsActive)
    {
        bIsActive = true;

        // 원래 플레이어 시점 저장
        OriginalControlRotation = PlayerController->GetControlRotation();
        OriginalLocation = PlayerController->GetPawn()->GetActorLocation();

        // CCTV 시점으로 전환
        PlayerController->SetViewTargetWithBlend(this, 0.0f);
    }
    else
    {
        bIsActive = false;
        PlayerController->SetViewTargetWithBlend(PlayerController->GetPawn(), 0.0f);
    }
}

EInteractionType ACCTV::GetInteractionType_Implementation()
{
    return EInteractionType::CCTV;
}
