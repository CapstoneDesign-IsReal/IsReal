// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AoE_Acid.generated.h"

class USphereComponent;
class APlayerCharacter;
class UNiagaraSystem;

UCLASS()
class ISREAL_API AAoE_Acid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAoE_Acid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAoEBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
							AActor* OtherActor, 
							UPrimitiveComponent* OtherComp, 
							int32 OtherBodyIndex, 
							bool bFromSweep, 
							const FHitResult& SweepResult);

	UFUNCTION()
	void OnAoEEndOverlap(UPrimitiveComponent* OverlappedComponent,
							AActor* OtherActor, 
							UPrimitiveComponent* OtherComp, 
							int32 OtherBodyIndex);

	void ApplyDamage();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AoE | Components")
	TObjectPtr<USphereComponent> SphereComp;		//handle collision

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AoE | VFX")
	TObjectPtr<UNiagaraSystem> AoEEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AoE | VFX")
	FVector EffectSize = FVector(1.0f, 1.0f, 1.0f);

	FTimerHandle DotDamageTimer;

	UPROPERTY(EditAnywhere, Category = "Params")
	float Damage = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Params")
	float Duration = 3.0f;	//duration of attack

	UPROPERTY(EditAnywhere, Category = "Params")
	float TickInterval = 1.0f;	//Tick interval of attack

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ExplodeSound;
};
