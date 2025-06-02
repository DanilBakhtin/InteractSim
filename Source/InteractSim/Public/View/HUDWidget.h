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

    /*Обновление информации об объектах в UI*/
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void UpdateInfoObjects(const TArray<FObjectData>& ObjectStates);

    // Динамическое событие нажатия кнопки "завершить сессию" 
    UPROPERTY(BlueprintAssignable, Category = "HUD Widget")
    FHUDWidgetEventSignature OnHUDClickCloseSession;

protected:
    // Функция для вызова события, вызывается в WBP_HUDWidget
    UFUNCTION(BlueprintCallable, Category = "HUD Widget")
    void TriggerCloseSessionEvent();
};
