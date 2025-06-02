// CopyRight: (c) Danil Bakhtin. All rights reserved

#include "View/InteractiveObject.h"

// Sets default values
AInteractiveObject::AInteractiveObject()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
}

void AInteractiveObject::ChangeColorObject()
{
    if (DynMaterial)
    {
        {
            FColor Color = ObjectState->GetColor();
            FVector LinearColorVector = FVector(Color.R / 255.0f, Color.G / 255.0f, Color.B / 255.0f);
            DynMaterial->SetVectorParameterValue("BaseColor", FLinearColor(LinearColorVector));
        }
    }
}

void AInteractiveObject::InitializeStaticMesh()
{
    FString PathToMesh;

    switch (ObjectState->ObjectType)
    {
    case EObjectType::Box:
        PathToMesh = TEXT("/Game/Meshes/SM_Cube.SM_Cube");
        break;
    case EObjectType::Sphere:
        PathToMesh = TEXT("/Game/Meshes/SM_Sphere.SM_Sphere");
        break;
    default:
        return;
    }

    // Загрузить меш во время выполнения
    UStaticMesh* MeshAsset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *PathToMesh));
    if (MeshAsset)
    {
        MeshComponent->SetStaticMesh(MeshAsset);
        MeshComponent->SetCollisionObjectType(ECC_GameTraceChannel1);

        UMaterialInterface* Material = MeshComponent->GetMaterial(0);
        if (Material)
            DynMaterial = UMaterialInstanceDynamic::Create(Material, this);

        if (DynMaterial)
            MeshComponent->SetMaterial(0, DynMaterial);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load mesh at path: %s"), *PathToMesh);
    }
}

void AInteractiveObject::Interact()
{
    ObjectState->bIsActive = !ObjectState->bIsActive;
    ChangeColorObject();
}

bool AInteractiveObject::GetActiveState()
{
    return ObjectState.IsValid() ? ObjectState->bIsActive : false;
}

void AInteractiveObject::InitializeFromData(TSharedPtr<FObjectData> Data)
{
    if (!Data)
        return;
    ObjectState = Data;
    
    InitializeStaticMesh();
    ChangeColorObject();
}
