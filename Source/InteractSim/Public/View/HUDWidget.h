// CopyRight: (c) Danil Bakhtin. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Model/DataManager.h>
#include "HUDWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHUDWidgetEventSignature);

UCLASS()
class INTERACTSIM_API UHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void HideAllText();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void ShowActivateText(bool bActivate);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void UpdateInfoObjects(const TArray<FObjectData>& ObjectStates);

    // ������������ �������, ������� ����� �������� �� Blueprint � ������������� �� ����
    UPROPERTY(BlueprintAssignable, Category = "HUD Widget")
    FHUDWidgetEventSignature OnHUDClickCloseSession; // ��� ����������

protected:
    // ������� ��� ������ ������� (����� ������� �� Blueprint)
    UFUNCTION(BlueprintCallable, Category = "HUD Widget")
    void TriggerCloseSessionEvent();
};
