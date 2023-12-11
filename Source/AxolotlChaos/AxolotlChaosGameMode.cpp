// Copyright Epic Games, Inc. All Rights Reserved.

#include "AxolotlChaosGameMode.h"
#include "AxolotlChaosCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAxolotlChaosGameMode::AAxolotlChaosGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_Axololt"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
