// Copyright (c) Danil Bakhtin. All rights reserved

#include "Model/DataManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "HAL/PlatformProcess.h"
#include "Kismet/KismetSystemLibrary.h"

bool UDataManager::LoadFromJSON(const FString& FilePath)
{
    // Универсальный путь для редактора и билда
    FString FullPath = GetUniversalSavePath(FilePath);

    // Логирование пути для отладки
    UE_LOG(LogTemp, Warning, TEXT("Loading from path: %s"), *FullPath);
    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Loading from: %s"), *FullPath), true, false,
                                      FLinearColor::Green, 10.0f);

    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *FullPath))
    {
        FString ErrorMsg = FString::Printf(TEXT("Failed to load JSON file: %s"), *FullPath);
        UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
        UKismetSystemLibrary::PrintString(this, ErrorMsg, true, false, FLinearColor::Red, 10.0f);
        return false;
    }

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    TSharedPtr<FJsonObject> RootObject;

    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        FString ErrorMsg = FString::Printf(TEXT("JSON parsing error: %s"), *FullPath);
        UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
        UKismetSystemLibrary::PrintString(this, ErrorMsg, true, false, FLinearColor::Red, 10.0f);
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* JsonArray;
    if (!RootObject->TryGetArrayField(TEXT("objects"), JsonArray))
    {
        FString ErrorMsg = TEXT("The 'objects' field was not found in JSON.");
        UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
        UKismetSystemLibrary::PrintString(this, ErrorMsg, true, false, FLinearColor::Red, 10.0f);
        return false;
    }

    ObjectStates.Empty();
    for (const TSharedPtr<FJsonValue>& Value : *JsonArray)
    {
        TSharedPtr<FObjectData> Data = MakeShared<FObjectData>();
        if (FJsonObjectConverter::JsonObjectToUStruct(Value->AsObject().ToSharedRef(), Data.Get()))
        {
            ObjectStates.Add(Data);
        }
    }

    FString SuccessMsg = FString::Printf(TEXT("Successfully loaded %d objects from JSON"), ObjectStates.Num());
    UE_LOG(LogTemp, Log, TEXT("%s"), *SuccessMsg);
    UKismetSystemLibrary::PrintString(this, SuccessMsg, true, false, FLinearColor::Green, 10.0f);

    return true;
}

bool UDataManager::SaveToJSON(const FString& FilePath)
{
    // Универсальный путь для редактора и билда
    FString FullPath = GetUniversalSavePath(FilePath);

    // Логирование пути для отладки
    UE_LOG(LogTemp, Warning, TEXT("Saving to path: %s"), *FullPath);
    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Saving to: %s"), *FullPath), true, false,
                                      FLinearColor::Yellow, 10.0f);

    // Создаем директорию, если ее нет
    FString Directory = FPaths::GetPath(FullPath);
    if (!IFileManager::Get().DirectoryExists(*Directory))
    {
        if (!IFileManager::Get().MakeDirectory(*Directory, true))
        {
            FString ErrorMsg = FString::Printf(TEXT("Failed to create directory: %s"), *Directory);
            UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
            UKismetSystemLibrary::PrintString(this, ErrorMsg, true, false, FLinearColor::Red, 10.0f);
            return false;
        }
    }

    // Сериализация данных
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> JsonArray;

    for (const TSharedPtr<FObjectData>& Data : ObjectStates)
    {
        TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(*Data);
        JsonArray.Add(MakeShareable(new FJsonValueObject(JsonObject)));
    }

    RootObject->SetArrayField("objects", JsonArray);

    // Сохранение в файл
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    if (!FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer))
    {
        FString ErrorMsg = TEXT("Failed to serialize JSON.");
        UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
        UKismetSystemLibrary::PrintString(this, ErrorMsg, true, false, FLinearColor::Red, 10.0f);
        return false;
    }

    if (!FFileHelper::SaveStringToFile(OutputString, *FullPath))
    {
        FString ErrorMsg = FString::Printf(TEXT("Failed to save JSON to file: %s"), *FullPath);
        UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
        UKismetSystemLibrary::PrintString(this, ErrorMsg, true, false, FLinearColor::Red, 10.0f);
        return false;
    }

    FString SuccessMsg = FString::Printf(TEXT("Successfully saved %d objects to: %s"), ObjectStates.Num(), *FullPath);
    UE_LOG(LogTemp, Log, TEXT("%s"), *SuccessMsg);
    UKismetSystemLibrary::PrintString(this, SuccessMsg, true, false, FLinearColor::Green, 10.0f);

    return true;
}

FString UDataManager::GetUniversalSavePath(const FString& FilePath) const
{
    FString BaseDir;

#if UE_BUILD_SHIPPING
    // Для shipping билда используем AppData/Local
    BaseDir = FPaths::Combine(FPlatformProcess::UserSettingsDir(), FApp::GetProjectName(), TEXT("Saved"));
#else
    // Для редактора и development билда используем стандартный Saved
    BaseDir = FPaths::ProjectSavedDir();
#endif

    // Добавляем указанный путь к файлу
    return FPaths::Combine(BaseDir, FilePath);
}