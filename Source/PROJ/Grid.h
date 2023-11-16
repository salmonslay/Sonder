// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridNode.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

class UBoxComponent;
class GridNode;
class APROJCharacter;

UCLASS()
class PROJ_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrid();

	TArray<FVector> RequestPath(FVector &Start, FVector &End);

	/** Timer handle to debug unwalkable nodes and grid, a lot of stuff in begin play can sometimes make nodes not being created fast enough*/
	FTimerHandle DebugDrawTimerHandle;

	/** Timer handel to start pathfinding, making sure grid and nodes are instantiated before calculations start*/
	FTimerHandle StartPathfindingTimerHandle;

	/** Method for switching bool to start pathfinding*/
	void OnStartPathfinding();

	/** bool to start pathfinding */
	bool bCanStartPathfinding = false;

	bool bAllNodesAdded = false;
	

	virtual ~AGrid() override;
	

	virtual void Tick(float DeltaSeconds) override;

	/** Returns the direction of node in specified location
	 * @param WorldLocation of the node to check
	 * @return Direction of node
	 */
	FVector GetDirectionBasedOnWorldLocation(const FVector WorldLocation) const { return GetNodeFromWorldLocation(WorldLocation)->GetDirection(); }
	
	/** Returns node on specified location
	 * @param NodeWorldLocation of the node
	 * @return pointer to Node in location
	 */
	GridNode* GetNodeFromWorldLocation(const FVector &NodeWorldLocation) const;

	/** Does what the method is called*/
	FVector GetGridBottomLeftLocation() const { return GridBottomLeftLocation; }

	/** Does what the method is called*/
	FVector GetGridSize() const { return GridSize; }

	FVector TargetLocation = FVector::ZeroVector;

	FVector StartLocation = FVector::ZeroVector;

	void OnDebugPathDraw(TArray<FVector> PathWaypoints);

	/** Gets the max size of grid */
	int GetGridMaxSize() const
	{
		return GridLengthX * GridLengthY * GridLengthZ;
	}

	void OnUpdatedPathFound();

	bool bHasUpdatedPath = false;

	void OnNoNeedUpdate();

	float GetNodeRadius() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UBoxComponent* GridBounds;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQueries;


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(VisibleAnywhere, Category="A*")
	float MovementPenalty = 400.f;
	

	APROJCharacter* Player1;
	APROJCharacter* Player2;

	// ============ General grid variables and funcs ============= //
	
	GridNode* Nodes; 

	/** Radius for each node, can be altered to smaller size for more accurate pathfinding but more performance expensive */
	UPROPERTY(EditAnywhere)
	float NodeRadius = 50.f;
	
	float NodeDiameter;  

	/** The size of the grid */
	FVector GridSize = FVector::ZeroVector;
	
	/** Array size x*/
	int GridLengthX = 0;
	/** Array size x*/
	int GridLengthY = 0;
	/** Array size x*/
	int GridLengthZ = 0;
	
	FVector GridBottomLeftLocation = FVector::ZeroVector; 


	friend class Pathfinder;
	Pathfinder* EnemyPathfinder = nullptr;

	UPROPERTY(EditAnywhere)
	bool bDebug = true; 
	
	void OnDebugDraw() const;
	
	void CreateGrid();

	void AddToArray(const int IndexX, const int IndexY, const int IndexZ, const GridNode Node);

	GridNode* GetNodeFromGrid(const int IndexX, const int IndexY, const int IndexZ) const;

	TArray<GridNode*> GetNeighbours(GridNode* Node) const;

	int GetIndex(const int IndexX, const int IndexY, const int IndexZ) const; 

	

};
