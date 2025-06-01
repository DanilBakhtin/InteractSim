// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractSimCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Model/DataManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AInteractSimCharacter::AInteractSimCharacter()
{
    check(GetCapsuleComponent());
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    check(GetCharacterMovement());
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    check(CameraBoom);
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;       // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    check(FollowCamera);
    FollowCamera->SetupAttachment(
        CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom
                                                      // adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false;    // Camera does not rotate relative to arm

    PrimaryActorTick.bCanEverTick = true;

    LengthLineTrace = 500.0f;

    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Widget/WBP_HUDWidget"));
    if (WidgetClassFinder.Succeeded())
    {
        InteractionWidgetClass = WidgetClassFinder.Class;
    }

    dataManager = NewObject<UDataManager>();

    PathToData = "ObjectsData.txt";
}

void AInteractSimCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitializingWidget();

    dataManager = NewObject<UDataManager>();
    LoadInteractiveObjectsFromFile();
}

void AInteractSimCharacter::BeginDestroy()
{
    if (InteractionWidget)
    {
        InteractionWidget->OnHUDClickCloseSession.RemoveDynamic(this, &AInteractSimCharacter::CloseGame);
    }

    Super::BeginDestroy();
}

void AInteractSimCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AInteractSimCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {

        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInteractSimCharacter::Move);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInteractSimCharacter::Look);

        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
                                           &AInteractSimCharacter::InteractWithInteractiveObject);

        EnhancedInputComponent->BindAction(ShowCursorAction, ETriggerEvent::Started, this,
                                           &AInteractSimCharacter::ChangeCursorVisabilty);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error,
               TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input "
                    "system. If you intend to use the legacy system, then you will need to update this C++ file."),
               *GetNameSafe(this));
    }
}

void AInteractSimCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AInteractSimCharacter::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AInteractSimCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CheckInteractiveObject();
}

void AInteractSimCharacter::InitializingWidget()
{
    if (InteractionWidgetClass)
    {
        InteractionWidget = Cast<UHUDWidget>(CreateWidget<UUserWidget>(GetWorld(), InteractionWidgetClass));

        if (InteractionWidget)
        {
            InteractionWidget->AddToViewport();
            InteractionWidget->SetVisibility(ESlateVisibility::Visible);
            InteractionWidget->OnHUDClickCloseSession.AddDynamic(this, &AInteractSimCharacter::CloseGame);
        }
    }
}

void AInteractSimCharacter::UpdateWidgetText()
{
    if (!InteractionWidget)
        return;

    if (CurrentInteractiveObject)
        InteractionWidget->ShowActivateText(!CurrentInteractiveObject->GetActiveState());
    else
        InteractionWidget->HideAllText();
}

void AInteractSimCharacter::UpdateWidgetInfoObjects()
{
    if (!InteractionWidget)
        return;

    InteractionWidget->UpdateInfoObjects(dataManager->ObjectStates);
}

void AInteractSimCharacter::LoadInteractiveObjectsFromFile()
{
    if (dataManager)
    {
        if (dataManager->LoadFromJSON(*PathToData))
        {   
            SpawnInteractiveObjects(dataManager->ObjectStates);
            UpdateWidgetInfoObjects();
        }        
    }
    
}

void AInteractSimCharacter::SpawnInteractiveObjects(TArray<FObjectData>& ObjectStates)
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    for (const FObjectData& Data : ObjectStates)
    {
        //GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Green, Data.ToString());

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        FVector Location = Data.Position;
        FRotator Rotation = FRotator::ZeroRotator;

        AInteractiveObject* NewObj =
            World->SpawnActor<AInteractiveObject>(AInteractiveObject::StaticClass(), Location, Rotation, SpawnParams);

        if (NewObj)
        {
            NewObj->InitializeFromData(Data);
        }
    }
}

void AInteractSimCharacter::ChangeCursorVisabilty()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();

    if (PC)
    {   
        const bool bShouldShowCursor = !PC->bShowMouseCursor;

        PC->bShowMouseCursor = bShouldShowCursor;

        if (bShouldShowCursor)
            PC->SetInputMode(FInputModeGameAndUI());
        else
            PC->SetInputMode(FInputModeGameOnly());

        PC->ShouldShowMouseCursor();
    }
}

void AInteractSimCharacter::CloseGame()
{
    if (dataManager)
        dataManager->SaveToJSON(*PathToData);

    if (GetWorld())
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            PC->ConsoleCommand("quit");
        }
    }
}

void AInteractSimCharacter::CheckInteractiveObject()
{
    FVector Start = FollowCamera->GetComponentLocation();
    FVector End = Start + FollowCamera->GetForwardVector() * LengthLineTrace;

    FHitResult Hit;
    FCollisionQueryParams Params;
    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams, Params);

    if (bHit)
    {
        AInteractiveObject* HitObject = Cast<AInteractiveObject>(Hit.GetActor());
        if (HitObject)
        {
            CurrentInteractiveObject = HitObject;
            UpdateWidgetText();
        }
        else
        {
            CurrentInteractiveObject = nullptr;
            UpdateWidgetText();
        }
    }
    else
    {
        CurrentInteractiveObject = nullptr;
        UpdateWidgetText();
    }

    // Визуализация трассы (опционально)
    // DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.0f, 0, 1.0f);
}

void AInteractSimCharacter::InteractWithInteractiveObject()
{
    if (CurrentInteractiveObject)
    {   
        dataManager->UpdateData(CurrentInteractiveObject->Interact());
        UpdateWidgetText();
        UpdateWidgetInfoObjects();
        //dataManager->SaveToJSON(*PathToData);
    }
}
