// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBox.h"

// Sets default values
AWeaponBox::AWeaponBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeaponBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBox::Interact_Implementation(AActor* Interactor)
{
	//SetWeaponType(EWeaponType::EWT_Rifle);
	//Destroy(); // 흠..Destroy가 있으면 한 몇초동안 아웃라이너에 남아있다가 사라짐
	// 이 사실을 가지고 한 몇초 플레이를 한다면 총알이 발사가 안됨. 
}

EInteractionType AWeaponBox::GetInteractionType_Implementation()
{
	//return EInteractionType::Box;
}
