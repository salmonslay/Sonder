// Fill out your copyright notice in the Description page of Project Settings.


#include "SonderSaveGame.h"

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
