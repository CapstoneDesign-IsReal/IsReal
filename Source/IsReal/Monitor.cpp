#include "Monitor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AMonitor::AMonitor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Root 생성
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // Static Mesh 생성
    MonitorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonitorMesh"));
    MonitorMesh->SetupAttachment(Root);

    // 스태틱 메시 불러오기 (예시: Engine 기본 Cube)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshAsset.Succeeded())
    {
        MonitorMesh->SetStaticMesh(MeshAsset.Object);
    }

    
}

void AMonitor::Interact_Implementation(AActor* Interactor)
{
    if (LinkedCCTV)
    {
        LinkedCCTV->Interact_Implementation(Interactor);
    }
}

EInteractionType AMonitor::GetInteractionType_Implementation()
{
    return EInteractionType::Monitor;
}
