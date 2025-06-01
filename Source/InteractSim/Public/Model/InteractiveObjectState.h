// CopyRight: (c) Danil Bakhtin. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InteractiveObjectState.generated.h"

UCLASS()
class INTERACTSIM_API UInteractiveObjectState : public UObject
{
    GENERATED_BODY()

public:
    UInteractiveObjectState();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bIsActive;

    FORCEINLINE FColor GetColor() const
    {
        return bIsActive ? FColor::Green : FColor::Red;
    }

protected:
    UPROPERTY(BlueprintReadOnly)
    int32 ID;

    UPROPERTY(BlueprintReadOnly)
    FString Name;

    UPROPERTY(BlueprintReadOnly)
    FVector Position;
};
