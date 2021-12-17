// Fill out your copyright notice in the Description page of Project Settings.


#include "LAva.h"
#include <MonProjet/MonProjetCharacter.h>

// Sets default values
ALAva::ALAva()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

}

// Called when the game starts or when spawned
void ALAva::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentHit.AddDynamic(this, &ALAva::OnHit);
	
}

// Called every frame
void ALAva::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALAva::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AMonProjetCharacter* Other = Cast<AMonProjetCharacter>(OtherActor);
	if (Other != nullptr) {
		Other->TakeDg(150);
	}
}

