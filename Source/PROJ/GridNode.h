#pragma once

#include "CoreMinimal.h"


class PROJ_API GridNode
{
public:

	/** Constructor with parameters*/
	GridNode(const bool bIsWalkable, const FVector WorldCoord, const int GridX, const int GridY, const int GridZ, int NewMovementPenalty)
		: bWalkable(bIsWalkable), WorldCoordinate(WorldCoord), GridX(GridX), GridY(GridY), GridZ(GridZ), MovementPenalty(NewMovementPenalty) {}
	
	/** Empty constructor*/
	GridNode() {}

	/** Sets nodes direction based on target node
	 * @param TargetNode node to point towards
	 */
	void SetDirection(const GridNode* TargetNode)
	{
		Direction = TargetNode->WorldCoordinate - WorldCoordinate;
		Direction.Normalize();
	}

	/** Gets nodes current direction, aka the direction of where entity should move toward when at current node
	 * @return FVector Direction of node
	 */
	FVector GetDirection() const { return Direction; }

	// =============  A* variables  ============== /
	/** Cost from the start node to this node */
	int GCost = 1;
	
	/** Heuristic cost from this node to the target nod */
	int HCost = 1;

	/** The sum of GCost and HCost */
	int FCost = 1;

	// ====== Getters ======= //
	bool IsWalkable() const { return bWalkable; }

	bool IsInOpenSet() const {return bIsInOpenSet; }

	bool IsInVisited() const { return bIsInVisited; }

	FVector GetWorldCoordinate() const { return WorldCoordinate; }

	int GetFlowFieldCost() const { return FlowCost; }

	int GetFCost() const { return GCost + HCost; }

	int GetGCost() const { return GCost; }

	int GetHCost() const { return HCost; }


	// ====== Setters ======= //

	void SetFlowFieldCost(const int NewCost) { FlowCost = NewCost; }

	void SetFCost(const int NewCost) { FCost = NewCost; }
	
	void SetGCost(const int NewCost) { GCost = NewCost; }

	void SetHCost(const int NewCost) { HCost = NewCost; }

	void SetIsInOpenSet(const bool NewValue) { bIsInOpenSet = NewValue;}
	void SetIsInVisited(const bool NewValue) { bIsInVisited = NewValue;}
	
	/** If the node is walkable */
	bool bWalkable = false;
	
	/** Nodes location in world space*/
	FVector WorldCoordinate;
	
	int GridX = -1;
	int GridY = -1;
	int GridZ = -1;

	int HeapIndex = 0;

	/** The node that leads to this node with the lowest FCost */
	GridNode* ParentNode = nullptr;

	int MovementPenalty = 1;
	
private:
	/** Nodes current direction, aka the direction of where entity should move toward when at current node*/
	FVector Direction = FVector::Zero();
	
	/** Cost to get to this node */
	int FlowCost = INT_MAX;
	
	/** Bool that indicates if node is in closed or open set, for a**/
	bool bIsInOpenSet = false;

	/** Bool that indicates if node is in closed or open set, for a**/
	bool bIsInVisited = false;
	
};
