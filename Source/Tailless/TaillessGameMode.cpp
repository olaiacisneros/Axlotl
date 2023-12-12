// Copyright Epic Games, Inc. All Rights Reserved.

<<<<<<< Updated upstream:Source/AxolotlChaos/AxolotlChaosGameMode.cpp
#include "AxolotlChaosGameMode.h"
#include "Axololt.h"
=======
#include "TaillessGameMode.h"
#include "TaillessCharacter.h"
>>>>>>> Stashed changes:Source/Tailless/TaillessGameMode.cpp
#include "UObject/ConstructorHelpers.h"

ATaillessGameMode::ATaillessGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_Axololt"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
