// Fill out your copyright notice in the Description page of Project Settings.

#include "SonderSaveGame.h"

#include "PROJCharacter.h"

ESonderLevel USonderSaveGame::GetHighestLevelCompleted() const
{
	ESonderLevel HighestLevel = ESonderLevel::None;
	for (const ESonderLevel Level : LevelsCompleted)
	{
		if (Level > HighestLevel)
		{
			HighestLevel = Level;
		}
	}
	return HighestLevel;
}

TMap<ESonderLevel, FString> USonderSaveGame::GetLevelPaths()
{
	TMap<ESonderLevel, FString> LevelPaths;
	LevelPaths.Add(ESonderLevel::Tutorial, TEXT("/Game/Maps/Levels/Tutorial"));
	LevelPaths.Add(ESonderLevel::Level1, TEXT("/Game/Maps/Levels/Level1"));
	LevelPaths.Add(ESonderLevel::Arena1, TEXT("/Game/Maps/Whiteboxes/Arena1Whitebox"));
	LevelPaths.Add(ESonderLevel::Level2, TEXT("/Game/Maps/Levels/Level2"));
	LevelPaths.Add(ESonderLevel::Arena2, TEXT("/Game/Maps/Whiteboxes/Arena2Whitebox"));
	LevelPaths.Add(ESonderLevel::Ending, TEXT("/Game/Maps/Whiteboxes/EndingWhitebox"));
	return LevelPaths;
}

FString USonderSaveGame::GetLevelToContinueTo(const ESonderLevel From)
{
	TMap<ESonderLevel, FString> LevelPaths = GetLevelPaths();

	switch (From) // this is fucking stupid
	{
	case ESonderLevel::None:
		return LevelPaths[ESonderLevel::Tutorial];
	case ESonderLevel::Tutorial:
		return LevelPaths[ESonderLevel::Level1];
	case ESonderLevel::Level1:
		return LevelPaths[ESonderLevel::Arena1];
	case ESonderLevel::Arena1:
		return LevelPaths[ESonderLevel::Level2];
	case ESonderLevel::Level2:
		return LevelPaths[ESonderLevel::Arena2];
	case ESonderLevel::Arena2:
		return LevelPaths[ESonderLevel::Ending];
	case ESonderLevel::Ending:
		return LevelPaths[ESonderLevel::Ending];
	default:
		return LevelPaths[ESonderLevel::Tutorial];
	}
}
