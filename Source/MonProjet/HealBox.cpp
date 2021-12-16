// Fill out your copyright notice in the Description page of Project Settings.


#include "HealBox.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include <MonProjet/MonProjetCharacter.h>

// Sets default values
AHealBox::AHealBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));
	RootComponent = Box;
	HealAmount = 5;

}

// Called when the game starts or when spawned
void AHealBox::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &AHealBox::OnOverlapBegin);
	
}

// Called every frame
void AHealBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AHealBox::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMonProjetCharacter* Other = Cast<AMonProjetCharacter>(OtherActor);
	if (Other != nullptr) {
		Other->AMonProjetCharacter::Healu(HealAmount);
	}
}
void AHealBox::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
