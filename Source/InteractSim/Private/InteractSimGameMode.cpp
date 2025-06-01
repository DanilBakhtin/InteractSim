// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractSimGameMode.h"
#include "InteractSimCharacter.h"
#include "UObject/ConstructorHelpers.h"

AInteractSimGameMode::AInteractSimGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
        TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}
