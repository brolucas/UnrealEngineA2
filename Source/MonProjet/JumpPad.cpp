// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPad.h"
#include <MonProjet/MonProjetCharacter.h>

// Sets default values
AJumpPad::AJumpPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AJumpPad::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentHit.AddDynamic(this, &AJumpPad::OnHit);
	
}

// Called every frame
void AJumpPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AJumpPad::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	AMonProjetCharacter* Other = Cast<AMonProjetCharacter>(OtherActor);
	if (Other != nullptr) {
		Other->LaunchCharacter(FVector(0, 0, 1000), false, false);
	}
}