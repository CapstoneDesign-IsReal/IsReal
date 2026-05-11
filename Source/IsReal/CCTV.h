#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Camera/CameraComponent.h"
#include "CCTV.generated.h"

class UUserWidget;

UCLASS()
class ISREAL_API ACCTV : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    ACCTV();

    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual EInteractionType GetInteractionType_Implementation() override;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCTV UI")
    TSubclassOf<UUserWidget> CCTVWidgetClass;

    UPROPERTY()
    UUserWidget* CCTVWidgetInstance = nullptr;

private:
    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* CCTVMesh;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* CCTVView;

    UPROPERTY(EditAnywhere, Category = "CCTV")
    float RotationSpeed = 90.f; // 카메라 회전 속도

    bool bIsActive = false;

    APlayerController* PlayerController;
    FRotator OriginalControlRotation;
    FVector OriginalLocation;

    void CheckExitInput();
    void Turn(float Value);
};
