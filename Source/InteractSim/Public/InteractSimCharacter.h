// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include <View/InteractiveObject.h>
#include <View/HUDWidget.h>
#include <Model/DataManager.h>
#include "InteractSimCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
class UDataManager;

struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AInteractSimCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AInteractSimCharacter();

    FORCEINLINE class USpringArmComponent* GetCameraBoom() const
    {
        return CameraBoom;
    }

    FORCEINLINE class UCameraComponent* GetFollowCamera() const
    {
        return FollowCamera;
    }

protected:
    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    UCameraComponent* FollowCamera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* ShowCursorAction;

    UPROPERTY()
    class AInteractiveObject* CurrentInteractiveObject;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive")
    float LengthLineTrace;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> InteractionWidgetClass;

    UHUDWidget* InteractionWidget;

    UPROPERTY()
    UDataManager* dataManager;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    FString PathToData;

    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Called for looking input */
    void Look(const FInputActionValue& Value);

    virtual void BeginPlay() override;

    virtual void BeginDestroy() override;

    UFUNCTION(BlueprintCallable)
    virtual void CheckInteractiveObject();

    UFUNCTION(BlueprintCallable)
    virtual void InteractWithInteractiveObject();

    virtual void NotifyControllerChanged() override;

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    virtual void Tick(float DeltaTime) override;

    virtual void InitializingWidget();

    virtual void UpdateWidgetText();

    virtual void UpdateWidgetInfoObjects();

    void LoadInteractiveObjectsFromFile();

    void SpawnInteractiveObjects(TArray<FObjectData>& ObjectStates);

    void ChangeCursorVisabilty();

    UFUNCTION()
    void CloseGame();
};
