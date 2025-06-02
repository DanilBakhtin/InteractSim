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

    /*�������������� � ��������, ����������� ��������� � ������ ����*/
    void Interact();

    /*��������� �������� ���������*/
    bool GetActiveState();

    /*������������� ��������� �������*/
    void InitializeFromData(TSharedPtr<FObjectData> Data);
    
protected:

    //������������ �������� ��� ����� �����
    UPROPERTY()
    UMaterialInstanceDynamic* DynMaterial;

    //������ �� ���������
    TSharedPtr<FObjectData> ObjectState;

    /*����� ����� � ���������*/
    void ChangeColorObject();

    /*������������� ����. ����� ������� � DataTable (���-���)�*/
    void InitializeStaticMesh();

};
