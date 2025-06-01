// CopyRight: (c) Danil Bakhtin. All rights reserved


#include "Model/InteractiveObjectState.h"

UInteractiveObjectState::UInteractiveObjectState()
{
    ID = 1;
    Name = TEXT("ActorState");
    Position = FVector(0, 0, 0);
    bIsActive = false;
}

