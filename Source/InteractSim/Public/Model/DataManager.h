// CopyRight: (c) Danil Bakhtin. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SharedPointer.h"
#include "DataManager.generated.h"

//Типы объектов (для выбора меша)
UENUM(BlueprintType)
enum class EObjectType : uint8
{
    Box UMETA(DisplayName = "Box"),
    Sphere UMETA(DisplayName = "Sphere"),
    Unknown UMETA(DisplayName = "Unknown")
};


//Структура для храниния информации об объекте
USTRUCT(BlueprintType)
struct FObjectData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 Id;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FString Name;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector Position;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FString Color;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool bIsActive;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
    EObjectType ObjectType;

    // Нужно для FJsonObjectConverter
    FObjectData() : Id(0), Position(FVector::ZeroVector), Color("green"), bIsActive(false)
    {
    }

    FORCEINLINE FColor GetColor() const
    {
        return bIsActive ? FColor::Green : FColor::Red;
    }

    FString ToString() const
    {
        const UEnum* EnumPtr = StaticEnum<EObjectType>();
        FString TypeStr = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(ObjectType)) : "Invalid";

        return FString::Printf(TEXT("Id: %d | Type: %s | Position: (%.1f, %.1f, %.1f) | Color: %s | IsActive: %s"), Id,
                               *TypeStr, Position.X, Position.Y, Position.Z, *Color,
                               bIsActive ? TEXT("true") : TEXT("false"));
    }
};

UCLASS()
class INTERACTSIM_API UDataManager : public UObject
{
    GENERATED_BODY()

public:
    
    TArray<TSharedPtr<FObjectData>> ObjectStates;

    /* Загрузка JSON из файла*/
    bool LoadFromJSON(const FString& FilePath);

    /* Сохранение JSON в файл*/
    bool SaveToJSON(const FString& FilePath);

};
