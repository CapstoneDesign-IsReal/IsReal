// Fill out your copyright notice in the Description page of Project Settings.

#include "AoE_Acid.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAoE_Acid::AAoE_Acid()
{
	PrimaryActorTick.bCanEverTick = false;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));

	RootComponent = SphereComp;
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	//prevent block player
}

// Called when the game starts or when spawned
void AAoE_Acid::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(Duration);
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAoE_Acid::OnAoEBeginOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &AAoE_Acid::OnAoEEndOverlap);

	float SphereRadius = SphereComp->GetScaledSphereRadius();
	float ScaleRatio = SphereRadius / 1200.0f;
	EffectSize = FVector(ScaleRatio, ScaleRatio, ScaleRatio);

	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.X += 15.0f;
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, SpawnLocation);
	if (AoEEffect) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AoEEffect, GetActorLocation(), GetActorRotation(), EffectSize);
	}

	
}

// Called every frame
void AAoE_Acid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAoE_Acid::OnAoEBeginOverlap(UPrimitiveComponent* OverlappedComponent,
									AActor* OtherActor,
									UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex,
									bool bFromSweep,
									const FHitResult& SweepResult)
{
	APlayerCharacter* OverlappedCharacter = Cast<APlayerCharacter>(OtherActor);	//prevent bug
	if (!OverlappedCharacter) return;

	PlayerCharacter = OverlappedCharacter;
	
	ApplyDamage();

	GetWorldTimerManager().SetTimer(DotDamageTimer, this, &AAoE_Acid::ApplyDamage, TickInterval, true);
}

void AAoE_Acid::OnAoEEndOverlap(UPrimitiveComponent* OverlappedComponent,
									AActor* OtherActor,
									UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex)
{
	if (OtherActor == PlayerCharacter) {
		GetWorldTimerManager().ClearTimer(DotDamageTimer);
		PlayerCharacter = nullptr;
	}
}

void AAoE_Acid::ApplyDamage()
{
	if (!IsValid(PlayerCharacter)) {
		GetWorldTimerManager().ClearTimer(DotDamageTimer);
		return;
	}
	
	PlayerCharacter->PlayerHit(Damage);
}

