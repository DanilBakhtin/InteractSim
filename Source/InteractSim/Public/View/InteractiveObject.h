// CopyRight: (c) Danil Bakhtin. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Templates/SharedPointer.h"
#include "Model/DataManager.h"
#include "InteractiveObject.generated.h"

class UObjectState;

UCLASS()
class INTERACTSIM_API AInteractiveObject : public AActor
{
    GENERATED_BODY()

public:

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComponent;

    AInteractiveObject();

    /*Взаимодействие с объектом, инвертируем состояние и меняем цвет*/
    void Interact();

    /*Получение текушего состояния*/
    bool GetActiveState();

    /*Инициализация состояния объекта*/
    void InitializeFromData(TSharedPtr<FObjectData> Data);
    
protected:

    //Динамический материал для смены цвета
    UPROPERTY()
    UMaterialInstanceDynamic* DynMaterial;

    //Ссылка на состояние
    TSharedPtr<FObjectData> ObjectState;

    /*Смена цвета в материале*/
    void ChangeColorObject();

    /*Инициализация меша. Лучше вынести в DataTable (тип-меш)я*/
    void InitializeStaticMesh();

};
