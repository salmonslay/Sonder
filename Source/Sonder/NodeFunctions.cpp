#include "NodeFunctions.h"

#include "GridNode.h"


namespace NodeFunctions
{
	int GetDistanceBetweenNodes(const GridNode* NodeA, const GridNode* NodeB)
	{
		const int DistanceX = FMath::Abs(NodeA->GridX - NodeB->GridX);
		const int DistanceY = FMath::Abs(NodeA->GridY - NodeB->GridY);
		const int DistanceZ = FMath::Abs(NodeA->GridZ - NodeB->GridZ);
		
		if (DistanceX >= DistanceY && DistanceX >= DistanceZ)
		{
			// X is the longest distance
			return 14 * DistanceY + 10 * (DistanceX - DistanceY) + 14 * DistanceZ;
		}
		if (DistanceY >= DistanceX && DistanceY >= DistanceZ)
		{
			// Y is the longest distance
			return 14 * DistanceX + 10 * (DistanceY - DistanceX) + 14 * DistanceZ;
		}
		// Z is the longest distance
		return 14 * DistanceX + 14 * DistanceY + 10 * (DistanceZ - DistanceX);
	}
	
	TArray<FVector> SimplifyPath(const TArray<GridNode*>& Path)
	{
		TArray<FVector> Waypoints;
		FVector DirectionOld = FVector::ZeroVector;

		for (int32 i = 1; i < Path.Num(); i++)
		{
			FVector DirectionNew = Path[i - 1]->GetWorldCoordinate() - Path[i]->GetWorldCoordinate();
			DirectionNew.Normalize();

			if (!DirectionNew.Equals(DirectionOld, KINDA_SMALL_NUMBER))
			{
				Waypoints.Add(Path[i]->GetWorldCoordinate());
			}
			DirectionOld = DirectionNew;
		}
		return Waypoints;
	}
	
	TArray<FVector> RetracePath(const GridNode* NodeA,  GridNode* NodeB)
	{
		TArray<GridNode*> Path;
		GridNode* Current = NodeB;

		while(Current != NodeA)
		{
			Path.Add(Current);
			Current = Current->ParentNode;
		}
		TArray<FVector> Waypoints = SimplifyPath(Path);
		return Waypoints;
	}

	bool IsMovingDiagonally(const GridNode* Current, const GridNode* Next)
	{
		return !(Current->GridX == Next->GridX || Current->GridY == Next->GridY || Current->GridZ == Next->GridZ); 
	}

	int GetCostToNode(const GridNode* Current, const GridNode* Next)
	{
		return IsMovingDiagonally(Current, Next) ? 14 : 10; 
	}
}
