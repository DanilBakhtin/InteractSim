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

    // Динамическое событие, которое можно вызывать из Blueprint и подписываться на него
    UPROPERTY(BlueprintAssignable, Category = "HUD Widget")
    FHUDWidgetEventSignature OnHUDClickCloseSession; // Без параметров

protected:
    // Функция для вызова события (можно вызвать из Blueprint)
    UFUNCTION(BlueprintCallable, Category = "HUD Widget")
    void TriggerCloseSessionEvent();
};
