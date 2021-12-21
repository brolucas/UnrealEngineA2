// Copyright Epic Games, Inc. All Rights Reserved.

#include "MonProjetCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"



//////////////////////////////////////////////////////////////////////////
// AMonProjetCharacter

AMonProjetCharacter::AMonProjetCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	Health = 100;
	JumpHeight = 600.f;
	RespawnLoc = FVector(-970.0f, -346.44342f, 202.000671f);
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SceneComponent->SetupAttachment(RootComponent);

	


	TraceDistance = 2000.0f;
}

//////////////////////////////////////////////////////////////////////////
// Input


void AMonProjetCharacter::TraceForward_Implementation()
{
	FVector Loc;
	FRotator Rot;
	FHitResult Hit(ForceInit);

	//Obtenir le point de vue du joueur
	GetController()->GetPlayerViewPoint(Loc, Rot);
	//Stocker ce point de vue au depart 

	FVector Start = Loc;
	FVector End = Start + (Rot.Vector() * TraceDistance);


	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), false, this);
	//ECC visibilité signifi que le trait sera sur le chanel visible quand il s'agir de colision ou encore sur la visibilté de la camera
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);


	//si on touche ou non
	if (bHit && Hit.GetActor()->IsRootComponentMovable())
	{
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);

		if (!isGrab)
		{

			FAttachmentTransformRules attach(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
			Hit.GetActor()->AttachToComponent(SceneComponent, attach, NAME_None);
			UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Hit.GetActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (StaticMesh == nullptr)
				return;

			StaticMesh->SetSimulatePhysics(false);

			isGrab = true;
		}
		else
		{
			FDetachmentTransformRules Dettach(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
			Hit.GetActor()->DetachAllSceneComponents(SceneComponent, Dettach);
			UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Hit.GetActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (StaticMesh == nullptr)
				return;

			StaticMesh->SetSimulatePhysics(true);

			isGrab = false;
		}
	}
}

void AMonProjetCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMonProjetCharacter::DoubleJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMonProjetCharacter::EndJump);
	// Pick Up
	PlayerInputComponent->BindAction("Pick_Up", IE_Pressed, this, &AMonProjetCharacter::InteractPressed);
	// TAG
	PlayerInputComponent->BindAction("Tag", IE_Pressed, this, &AMonProjetCharacter::Tag);


	PlayerInputComponent->BindAxis("MoveForward", this, &AMonProjetCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMonProjetCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMonProjetCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMonProjetCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMonProjetCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMonProjetCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMonProjetCharacter::OnResetVR);
}

void AMonProjetCharacter::Landed(const FHitResult& Hit)
{
	DoubleJumpCOunter = 0;
}




void AMonProjetCharacter::Tag()
{
	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AProj* Projectile = World->SpawnActor<AProj>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}

void AMonProjetCharacter::OnResetVR()
{
	// If MonProjet is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in MonProjet.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMonProjetCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMonProjetCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMonProjetCharacter::InteractPressed()
{
	TraceForward();
}

void AMonProjetCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMonProjetCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMonProjetCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMonProjetCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
void AMonProjetCharacter::TakeDg(int deg) {
	Health -= deg;

	if (Health <= 0) {
		Kill();
	}

}
void AMonProjetCharacter::Healu(int Amount) {
	Health += Amount;
}

void AMonProjetCharacter::DoubleJump()
{
	if (DoubleJumpCOunter <= 1) {
		ACharacter::LaunchCharacter(FVector(0, 0, JumpHeight), false, true);
		DoubleJumpCOunter++;
	}
	IsJumping = true;
	
}

void AMonProjetCharacter::EndJump()
{
	StopJumping();
	IsJumping = false;
}

void AMonProjetCharacter::Kill()
{
	//Destroy();
	Respawn();
}

void AMonProjetCharacter::Respawn()
{
	//SpawnDefaultController();
	Health = 100;
	SetActorLocation(RespawnLoc);
}


