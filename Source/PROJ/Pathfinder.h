#pragma once

#include "CoreMinimal.h"
#include "PROJCharacter.h"


class GridNode;
class AGrid;
class APROJCharacter;

class PROJ_API Pathfinder
{
public:

	Pathfinder(APROJCharacter* ServerPlayer, APROJCharacter* ClientPlayer, AGrid* MapGrid);
	
	

	/** Method for A*, calculating path from a start location to target location
	 * Attempts to find a path from a given start location to an end location. Uses A* algorithm, and if a path is found, it stores the path's waypoints and returns true.
	 */
	bool FindPath(const FVector &Start, const FVector &End);

	/** Latest path calculated by FindPath*/
	TArray<FVector> WayPoints;

	/** Stores the latest viable path, if no path is found entity can follow the last known one until a path can be found*/
	TArray<FVector> OldWayPoints;

	/** Indicating if path has been found*/
	bool bHasPath = false;
	
	bool bIsOldPathStillValid = false;

	FVector TargetLocation = FVector::ZeroVector;
private:
	

	APROJCharacter* ServerPlayerCharacter;

	APROJCharacter* ClientPlayerCharacter;

	/** If target not moving, makes it to skip flowfield pathfinding */
	GridNode* OldTargetNode = nullptr;

	/** If entity not moving, makes it to skip a* pathfinding */
	GridNode* OldStartNode = nullptr;

	/** If target not moving, makes it to skip a* pathfinding */
	GridNode* OldEndNode = nullptr;

	/** Pointer to grid class*/
	AGrid* Grid;	
};
