// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile_AcidBomb.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraFunctionLibrary;

UCLASS(BlueprintType, Blueprintable)
class ISREAL_API AProjectile_AcidBomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile_AcidBomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile | Components")
	TObjectPtr<USphereComponent> SphereComp;							//handle collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile | Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;	//handle projection of object
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile | Components")
	TObjectPtr<UStaticMeshComponent> ProjectileMeshComp;				//Setting Projectile Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile | VFX")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile | VFX")
	FVector ExplosionSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile | VFX")
	FRotator ExplosionRotation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Projectile | Physics")
	float PhysicsForce = 100.0f;

	//functions
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
