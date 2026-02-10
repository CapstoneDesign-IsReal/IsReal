#include "Card.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ACard::ACard()
{
    PrimaryActorTick.bCanEverTick = false;

    CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardMesh"));
    SetRootComponent(CardMesh);
}

void ACard::Interact_Implementation(AActor* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("Card Picked Up"));

    
    
    Destroy(); 
}

EInteractionType ACard::GetInteractionType_Implementation()
{
    return EInteractionType::Card;
}
