// Fill out your copyright notice in the Description page of Project Settings.


#include "SonderGameInstance.h"

USonderGameInstance::USonderGameInstance(const FObjectInitializer& ObjectInitializer) : UAdvancedFriendsGameInstance(ObjectInitializer)
{
	//TODO: Change file name to dynamic with time stamp
	FilePath = FPaths::ConvertRelativePathToFull(FPaths::GameAgnosticSavedDir() + TEXT("/LoggedData.txt"));

	FString SessionHead = "New Session Started " + FDateTime::UtcNow().ToString();
	FFileHelper::SaveStringToFile(SessionHead + "\n", *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}

void USonderGameInstance::AddToLog(FString TextToLog)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(FilePath));

	const FDateTime Now = FDateTime::UtcNow();
	FFileHelper::SaveStringToFile(Now.ToString() + TextToLog + "\n", *FilePath ,FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}

void USonderGameInstance::LoadNewLevel(const ESonderLevel NewLevel)
{
	PreviousLevel = CurrentLevel; 
	CurrentLevel = NewLevel; 

	GetWorld()->ServerTravel(*USonderSaveGame::GetLevelPaths().Find(NewLevel)); 
}
