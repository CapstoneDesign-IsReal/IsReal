#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"      // 인터페이스
#include "CCTV.h"              // LinkedCCTV 사용을 위한 include
#include "Monitor.generated.h"

UCLASS()
class ISREAL_API AMonitor : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    AMonitor();

    // 플레이어가 상호작용할 때 호출
    virtual void Interact_Implementation(AActor* Interactor) override;

    virtual EInteractionType GetInteractionType_Implementation() override;

    // 이 모니터가 연결된 CCTV (에디터에서 드래그해서 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ACCTV* LinkedCCTV;

protected:

    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MonitorMesh;

    UPROPERTY(EditAnywhere, Category = "Monitor")
    UMaterialInterface* MonitorMaterial;
};
