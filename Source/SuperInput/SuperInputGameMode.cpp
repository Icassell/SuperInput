// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperInputGameMode.h"
#include "SuperInputCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASuperInputGameMode::ASuperInputGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
