#pragma once

class GridNode;

namespace NodeFunctions
{
	/** Gets distance between two nodes based on their grid values  */
	int GetDistanceBetweenNodes(const GridNode* NodeA, const GridNode* NodeB);

	/** Returns simplified path of vectors, removing redundant waypoints with minimal direction changes, helper to retrace path*/
	TArray<FVector> SimplifyPath(const TArray<GridNode*>& Path);

	/** Returns path of vectors from a given destination node (NodeB) back to a starting node (NodeA) and returns the simplified waypoints of this path using SimplifyPath func*/
	TArray<FVector> RetracePath(const GridNode* NodeA,  GridNode* NodeB);

	/**Checks if moving from Current to NextNode would be a diagonal move, if any coordinate is same in Next och Current there is no diagonal move */
	bool IsMovingDiagonally(const GridNode* Current, const GridNode* Next);

	/** Gets cost from current to next*/
	int GetCostToNode(const GridNode* Current, const GridNode* Next);
	
}
