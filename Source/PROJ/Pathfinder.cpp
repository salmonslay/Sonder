﻿#include "Pathfinder.h"

#include "Grid.h"
#include "NodeFunctions.h"
#include "PathfinderHeap.h"
#include "PROJCharacter.h"


Pathfinder::Pathfinder(APawn* ServerPlayerPawn, APawn* ClientPlayerPawn, AGrid* MapGrid) : ServerPlayerPawn(ServerPlayerPawn), ClientPlayerPawn(ClientPlayerPawn), Grid(MapGrid)
{
	ServerPlayerCharacter = Cast<APROJCharacter>(ServerPlayerPawn);
	ClientPlayerCharacter = Cast<APROJCharacter>(ClientPlayerPawn);
	ensure(ServerPlayerCharacter != nullptr);
	ensure(ClientPlayerCharacter != nullptr);

	
}

bool Pathfinder::FindPath(const FVector &Start, const FVector &End)
{
	TArray<FVector> NewPath;
	//if (Grid->TargetLocation == FVector::ZeroVector || Grid->StartLocation == FVector::ZeroVector)
	if (Start == FVector::ZeroVector || End == FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Warning, TEXT("locations are 0"));
		return false;
	}

	GridNode* StartNode = Grid->GetNodeFromWorldLocation(Start);
	GridNode* EndNode = Grid->GetNodeFromWorldLocation(End);
	
	ensure(StartNode != nullptr);
	ensure(EndNode != nullptr);
	
	//If target and entity has not moved, updating path is redundant
	if(EndNode == OldEndNode && StartNode == OldStartNode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Didnt need updating path"));
		bIsOldPathStillValid = true;
		Grid->OnNoNeedUpdate();
		return false;
	}
	
	OldEndNode = EndNode;
	OldStartNode = StartNode;

	StartNode->ParentNode = StartNode;
	StartNode->SetGCost(0.f);
	StartNode->SetFCost(0.f);
	StartNode->SetHCost(0.f);

	bool bPathSuccess = false;

	// here you could check that start node is walkable as well, but because the flocking entities have a float-like behaviour sometimes
	// they wouldn't be able to get out of collision if they were in one. 
	if (EndNode->bWalkable)
	{
		// Create open and closed sets, should probably be something else or not created every time
		PathfinderHeap OpenSet = PathfinderHeap(Grid->GetGridMaxSize());
		TSet<GridNode*> ClosedSet;

		OpenSet.Add(StartNode);
		StartNode->SetIsInOpenSet(true);

		while (OpenSet.Count() > 0)
		{
	        // Find the node in the open set with the lowest FCost
			GridNode* CurrentNode = OpenSet.RemoveFirst();
			
			ClosedSet.Add(CurrentNode);

	        // If the current node is the end node, path found
	        if (CurrentNode == EndNode)
	        {
	        	bPathSuccess = true;
	        	bIsOldPathStillValid = false;
	        	UE_LOG(LogTemp, Warning, TEXT("New PathFound"));
				break;
	        }

	        // Iterate through the neighbors of the current node
	        for (GridNode* Neighbour : Grid->GetNeighbours(CurrentNode))
	        {
	            // Skip unwalkable or nodes in the closed set
	        	ensure (Neighbour != nullptr);
	            if (!Neighbour->IsWalkable() || ClosedSet.Contains(Neighbour))
	            {
	                continue;
	            }
	        	
	            // Calculate tentative GCost from the start node to this neighbor
	            const int NewMovementCostToNeighbour = CurrentNode->GetGCost() + NodeFunctions::GetDistanceBetweenNodes(CurrentNode, Neighbour) + CurrentNode->MovementPenalty;
	            // If this path is better than the previous one, update the neighbor
	            if (NewMovementCostToNeighbour < Neighbour->GetGCost() || !OpenSet.Contains(Neighbour))
	            {
	                Neighbour->SetGCost(NewMovementCostToNeighbour);
	                Neighbour->SetHCost(NodeFunctions::GetDistanceBetweenNodes(Neighbour, EndNode));
	                Neighbour->SetFCost(Neighbour->GetGCost() + Neighbour->GetHCost());
	                Neighbour->ParentNode = CurrentNode;

	                // Add the neighbor to the open set if not already there
	                if (!OpenSet.Contains(Neighbour))
	                {
	                	OpenSet.Add(Neighbour);
	                }
            		else
            		{
            			OpenSet.UpdateItem(Neighbour);
            		}
	            }
	        }
		}
	}
	if (bPathSuccess)
	{
		bHasPath = true;
		OldWayPoints = WayPoints;
		WayPoints = NodeFunctions::RetracePath(StartNode, EndNode);
		return true;
	}
	bHasPath = false;
	return false;
}
void Pathfinder::ResetFlowFieldNodeCosts() const
{
	for(int x = 0; x < Grid->GridLengthX; x++)
	{
		for(int y = 0; y < Grid->GridLengthY; y++)
		{
			for(int z = 0; z < Grid->GridLengthZ; z++)
			{
				Grid->GetNodeFromGrid(x, y, z)->SetFlowFieldCost(INT_MAX);
			}
		}
	}
}

void Pathfinder::SetDirectionInUnWalkableNode(GridNode* NeighbourNode) const 
{
	// to find lowest cost, set lowest cost to max value
	int LowestCostAwayFromNode = INT_MAX;
	// check all neighbours. If neighbour has a lower cost than lowest and is walkable, set direction to it
	for (const GridNode* NeighboursNeighbour : Grid->GetNeighbours(NeighbourNode))
	{
		const int CostToNeighbour = NodeFunctions::GetCostToNode(NeighbourNode, NeighboursNeighbour);
		if (NeighboursNeighbour->IsWalkable() && CostToNeighbour < LowestCostAwayFromNode)
		{
			NeighbourNode->SetDirection(NeighboursNeighbour);
			LowestCostAwayFromNode = CostToNeighbour;
		}
	}
}