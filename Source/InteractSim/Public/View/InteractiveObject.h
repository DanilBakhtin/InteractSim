// CopyRight: (c) Danil Bakhtin. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Model/InteractiveObjectState.h>
#include <Model/DataManager.h>
#include "InteractiveObject.generated.h"

class UObjectState;

UCLASS()
class INTERACTSIM_API AInteractiveObject : public AActor
{
    GENERATED_BODY()

public:
    AInteractiveObject();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    FObjectData Interact();
    bool GetActiveState();

    void InitializeFromData(const FObjectData& Data);

private:

    void ChangeColorObject();

    void InitializeStaticMesh();

public:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComponent;

private:
    UPROPERTY()
    UMaterialInstanceDynamic* DynMaterial;

    UPROPERTY()
    FObjectData ObjectState;

};
