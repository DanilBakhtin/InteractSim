// CopyRight: (c) Danil Bakhtin. All rights reserved

#include "Model/DataManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"

bool UDataManager::LoadFromJSON(const FString& FilePath)
{
    FString FullPath = FPaths::ProjectSavedDir() + FilePath;
    FString JsonString;

    if (!FFileHelper::LoadFileToString(JsonString, *FullPath))
    {
        //UE_LOG(LogTemp, Error, TEXT("Не удалось загрузить файл JSON: %s"), *FullPath);
        GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Red,
                                         FString::Printf(TEXT("Failed to upload JSON file: %s"), *FullPath));
        return false;
    }

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    TSharedPtr<FJsonObject> RootObject;

    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        // UE_LOG(LogTemp, Error, TEXT("Ошибка парсинга JSON: %s"), *FullPath);
        GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Red,
                                         FString::Printf(TEXT("JSON parsing error: %s"), *FullPath));
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* JsonArray;
    if (!RootObject->TryGetArrayField(TEXT("objects"), JsonArray))
    {
        // UE_LOG(LogTemp, Error, TEXT("Поле 'objects' не найдено в JSON."));
        GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Red,
                                         TEXT("The 'objects' field was not found in JSON."));
        return false;
    }

    ObjectStates.Empty();
    for (const TSharedPtr<FJsonValue>& Value : *JsonArray)
    {
        FObjectData Data;
        if (FJsonObjectConverter::JsonObjectToUStruct(Value->AsObject().ToSharedRef(), &Data))
        {
            ObjectStates.Add(Data);
        }
        else
            GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Red,
                                             TEXT("Error parse object."));
    }

    // UE_LOG(LogTemp, Log, TEXT("Успешно загружено %d объектов из JSON."), ObjectStates.Num());

    GEngine->AddOnScreenDebugMessage(
        INDEX_NONE, 10.0f, FColor::Green,
        FString::Printf(TEXT("Successfully loaded %d objects from JSON"), ObjectStates.Num()));
    return true;
}

bool UDataManager::SaveToJSON(const FString& FilePath)
{
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);

    TArray<TSharedPtr<FJsonValue>> JsonArray;
    for (const FObjectData& Data : ObjectStates)
    {
        TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(Data);
        JsonArray.Add(MakeShareable(new FJsonValueObject(JsonObject)));
    }

    RootObject->SetArrayField("objects", JsonArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    if (!FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer))
    {
        UE_LOG(LogTemp, Error, TEXT("Не удалось сериализовать JSON."));
        return false;
    }

    FString FullPath = FPaths::ProjectSavedDir() + FilePath;
    if (!FFileHelper::SaveStringToFile(OutputString, *FullPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Не удалось сохранить JSON в файл: %s"), *FullPath);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Успешно сохранено %d объектов в файл: %s"), ObjectStates.Num(), *FullPath);
    GEngine->AddOnScreenDebugMessage(
        INDEX_NONE, 10.0f, FColor::Green,
        FString::Printf(TEXT("Successfully saved %d objects to a file: %s"), ObjectStates.Num(), *FullPath));
    return true;
}

void UDataManager::UpdateData(const FObjectData& Data)
{
    for (int i = 0; i < ObjectStates.Num(); i++)
    {
        if (ObjectStates[i].Id == Data.Id)
        {
            ObjectStates[i] = Data;
        }
    }
}
