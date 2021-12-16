// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageBox.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include <MonProjet/MonProjetCharacter.h>

// Sets default values
ADamageBox::ADamageBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));
	RootComponent = Box;
	damage = 5;
}

// Called when the game starts or when spawned
void ADamageBox::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &ADamageBox::OnOverlapBegin);

	
}

// Called every frame
void ADamageBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ADamageBox::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMonProjetCharacter* Other = Cast<AMonProjetCharacter>(OtherActor);
	if (Other != nullptr) {
		Other->AMonProjetCharacter::TakeDg(damage);
	}
}
void ADamageBox::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

