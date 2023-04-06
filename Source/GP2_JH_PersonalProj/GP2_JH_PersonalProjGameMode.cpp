// Copyright Epic Games, Inc. All Rights Reserved.

#include "GP2_JH_PersonalProjGameMode.h"
#include "GP2_JH_PersonalProjCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGP2_JH_PersonalProjGameMode::AGP2_JH_PersonalProjGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
