// Copyright Epic Games, Inc. All Rights Reserved.

#include "MonProjetGameMode.h"
#include "MonProjetCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMonProjetGameMode::AMonProjetGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
