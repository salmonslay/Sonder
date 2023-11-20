// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"

#include "EnemyCharacter.h"
#include "Pathfinder.h"
#include "SonderGameState.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"



// Sets default values
AGrid::AGrid()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
		PrimaryActorTick.bCanEverTick = true;

		GridBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("GridBounds"));
	}
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
	NodeDiameter = NodeRadius * 2;

	CreateGrid();
	
	
	if(bDebug)
	{
		GetWorldTimerManager().SetTimer(DebugDrawTimerHandle, this, &AGrid::OnDebugDraw, 0.2f, false, -1);
	}

	//GetWorldTimerManager().SetTimer(StartPathfindingTimerHandle, this, &AGrid::OnStartPathfinding, 0.1f, false, 0.1f);
	GetWorldTimerManager().SetTimer(CheckOverlappingEnemiesTimerHandle, this, &AGrid::CheckGridBoundOverlappingActors, CheckOverlappingEnemiesDelay, false, -1 );
	GetWorldTimerManager().SetTimer(CheckForPlayersTimerHandle, this, &AGrid::CheckForPlayers, CheckForPlayersLoopDelay, true, 0.1f); 
}

void AGrid::CreatePathfinder()
{
	EnemyPathfinder = new Pathfinder(ServerPlayer, ClientPlayer,this);
	bHasPathfinder = true;
}

void AGrid::CheckForPlayers()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		ServerPlayer = Cast<ASonderGameState>(GetWorld()->GetGameState())->GetServerPlayer();
		ClientPlayer = Cast<ASonderGameState>(GetWorld()->GetGameState())->GetClientPlayer();
	}
	if(ServerPlayer && ClientPlayer)
	{
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Grid has set both players"));
		}
		bHasFoundBothPlayers = true;
		GetWorldTimerManager().ClearTimer(CheckForPlayersTimerHandle);
	}
}

void AGrid::OnDebugPathDraw() const
{
	if (CurrentPath.Num() != 0)
	{
		for(const FVector Loc  : CurrentPath)
		{
			const GridNode* Node = GetNodeFromWorldLocation(Loc); 
			ensure(Node != nullptr);
			DrawDebugSphere(GetWorld(), Node->GetWorldCoordinate(), NodeRadius, 10, FColor::Green, false, 0.1);
		}
	}

	const GridNode*  Node = GetNodeFromWorldLocation(TargetLocation); 
	DrawDebugSphere(GetWorld(), Node->GetWorldCoordinate(), NodeDiameter, 10, FColor::Cyan, false, 0.1);

	const GridNode* StartNode = GetNodeFromWorldLocation(StartLocation); 
	DrawDebugSphere(GetWorld(), StartNode->GetWorldCoordinate(), NodeDiameter, 10, FColor::Purple, false, 0.1);
}

AGrid::~AGrid()
{
	delete [] Nodes;
	delete EnemyPathfinder;
}

// Called every frame,
//TODO: make own, better tick
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bAllNodesAdded)
	{
		return;
	}

	if (!bHasFoundBothPlayers)
	{
		return;
	}
	if (!bHasPathfinder)
	{
		CreatePathfinder();
	}
}


TArray<FVector> AGrid::RequestPath(const FVector &Start, const FVector &End, const bool bDoDebugDraw)
{
	TArray<FVector> NewPath;

	GridNode* StartNode = GetNodeFromWorldLocation(Start);
	GridNode* EndNode = GetNodeFromWorldLocation(End);
	
	if (EnemyPathfinder->FindPath(StartNode->GetWorldCoordinate(), EndNode->GetWorldCoordinate()))
	{
		NewPath = EnemyPathfinder->WayPoints;
	}
	else
	{
		NewPath = EnemyPathfinder->OldWayPoints;
	}
	
	Algo::Reverse(NewPath);
	if (bDoDebugDraw)
	{
		OnDebugPathDraw();
	}
	CurrentPath = NewPath;
	return NewPath;
}



void AGrid::OnUpdatedPathFound()
{
	if (EnemyPathfinder->bHasPath)
	{
		if (!EnemyPathfinder->bIsOldPathStillValid)
		{
			if (bDebug)
			{
				UE_LOG(LogTemp, Warning, TEXT("Updated path in grid"));
			}
			bHasUpdatedPath = true;
		}
	}
}


void AGrid::OnNoNeedUpdate()
{
	bHasUpdatedPath = false;
}

float AGrid::GetNodeRadius() const 
{
	return NodeRadius;
}



void AGrid::CreateGrid()
{
	GridSize.X = GridBounds->GetScaledBoxExtent().X * 2;
	GridSize.Y = GridBounds->GetScaledBoxExtent().Y * 2;
	GridSize.Z = GridBounds->GetScaledBoxExtent().Z * 2;

	GridLengthX = FMath::RoundToInt(GridSize.X / NodeDiameter);
	GridLengthY = FMath::RoundToInt(GridSize.Y / NodeDiameter);
	GridLengthZ = FMath::RoundToInt(GridSize.Z / NodeDiameter);
	

	Nodes = new GridNode[GridLengthX * GridLengthY * GridLengthZ]; 

	if (bDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid sizeX = %f, Y = %f, Z = %f"), GridSize.X, GridSize.Y, GridSize.Z );
		UE_LOG(LogTemp, Warning, TEXT("Bounds sizeX = %f, Y = %f, Z = %f"), GridBounds->GetScaledBoxExtent().X * 2, GridBounds->GetScaledBoxExtent().Y * 2, GridBounds->GetScaledBoxExtent().Z * 2);
		UE_LOG(LogTemp, Warning, TEXT("Grid nodes length X = %i, Y = %i, Z = %i"), GridLengthX, GridLengthY, GridLengthZ );
	}
	
	// The grid's pivot is in the center, need its position as if pivot was in the bottom left corner

	

	//FVector GridBottomLeft = GetActorLocation();
	//GridBottomLeft.X -= GridSize.X / 2;
	//GridBottomLeft.Y -= GridSize.Y / 2;
	//GridBottomLeft.Z -= GridSize.Z / 2;



	
	FVector GridTopLeft = GetActorLocation();
	GridTopLeft.X -= GridSize.X / 2;
	GridTopLeft.Y -= GridSize.Y / 2;
	GridTopLeft.Z += GridSize.Z / 2;
	

	//GridBottomLeftLocation = GridBottomLeft; 
	GridTopLeftLocation = GridTopLeft;

	//AActor* OverlapActor = GetWorld()->SpawnActor<AActor>(OverlapCheckActorClass, GetActorLocation(),FRotator::ZeroRotator); 
	
	for(int x = 0; x < GridLengthX; x++)
	{
		for(int y = 0; y < GridLengthY; y++)
		{
			for(int z = 0; z < GridLengthZ; z++)
			{
				//FVector NodePos = GridBottomLeft;
				FVector NodePos = GridTopLeft;
				NodePos.X += x * NodeDiameter + NodeRadius; 
				NodePos.Y += y * NodeDiameter + NodeRadius;
				//NodePos.Z += z * NodeDiameter + NodeRadius;
				NodePos.Z -= z * NodeDiameter + NodeRadius;

				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(this);
				TArray<AActor*> OverlappingActors;
				
				UKismetSystemLibrary::SphereOverlapActors(GetWorld(), NodePos, NodeRadius, ObjectTypeQueries, nullptr, ActorsToIgnore, OverlappingActors );
				
				int NewMovementPenalty = OverlappingActors.IsEmpty() ? 0 : MovementPenalty;
				AddToArray(x, y, z, GridNode(OverlappingActors.IsEmpty(), NodePos,  x, y, z, NewMovementPenalty));
			}
		}
	}
	bAllNodesAdded = true;
	if (bDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Done creating grid"));
	}
}

void AGrid::AddToArray(const int IndexX, const int IndexY, const int IndexZ, const GridNode Node)  
{
	Nodes[GetIndex(IndexX, IndexY, IndexZ)] = Node;
	if (!Node.bWalkable)
	{
		Nodes[GetIndex(IndexX, IndexY, IndexZ + 2)].bWalkable = false;
	}
}

GridNode* AGrid::GetNodeFromGrid(const int IndexX, const int IndexY, const int IndexZ) const
{
	return &Nodes[GetIndex(IndexX, IndexY, IndexZ)];
}

TArray<GridNode*> AGrid::GetNeighbours(GridNode* Node) const
{
	TArray<GridNode*> Neighbours;

	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			for(int z = -1; z <= 1; z++)
			{
				if(x == 0 && y == 0 && z == 0) 
					continue;

				const int GridX = Node->GridX + x; 
				const int GridY = Node->GridY + y;
				const int GridZ = Node->GridZ + z;
				
				// if any index is out of bounds 
				if(GridX < 0 || GridX > GridLengthX - 1 || GridY < 0 || GridY > GridLengthY - 1 || GridZ < 0 || GridZ > GridLengthZ - 1) continue;

				Neighbours.Add(GetNodeFromGrid(GridX, GridY, GridZ)); 
			}
		}
	}
	return Neighbours; 
}

int AGrid::GetIndex(const int IndexX, const int IndexY, const int IndexZ) const
{
	return (IndexX * (GridLengthY * GridLengthZ)) + (IndexY * GridLengthZ) + IndexZ;
}


GridNode* AGrid::GetNodeFromWorldLocation(const FVector &NodeWorldLocation) const
{
	// position relative to grids bottom left corner 
	//const float GridRelativeX = NodeWorldLocation.X  - GridBottomLeftLocation.X; 
	//const float GridRelativeY = NodeWorldLocation.Y - GridBottomLeftLocation.Y;
	//const float GridRelativeZ = NodeWorldLocation.Z - GridBottomLeftLocation.Z;

	const float GridRelativeX = NodeWorldLocation.X  - GridTopLeftLocation.X; 
	const float GridRelativeY = NodeWorldLocation.Y - GridTopLeftLocation.Y;
	const float GridRelativeZ = GridTopLeftLocation.Z - NodeWorldLocation.Z;
	
	// checks how many nodes fit in relative position for array indexes 
	const int x = FMath::Clamp(FMath::RoundToInt((GridRelativeX - NodeRadius) / NodeDiameter), 0, GridLengthX - 1);
	const int y = FMath::Clamp(FMath::RoundToInt((GridRelativeY - NodeRadius) / NodeDiameter), 0, GridLengthY - 1);
	const int z = FMath::Clamp(FMath::RoundToInt((GridRelativeZ - NodeRadius) / NodeDiameter), 0, GridLengthZ- 1);
	
	return GetNodeFromGrid(x, y, z);
}

void AGrid::CheckGridBoundOverlappingActors()
{
	TArray<AActor*> OverlappingActors;
	GridBounds->GetOverlappingActors(OverlappingActors, AEnemyCharacter::StaticClass());
	
	if (!OverlappingActors.IsEmpty())
	{
		for (AActor* OverlappingActor : OverlappingActors)
		{
			AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OverlappingActor);
			if (EnemyCharacter != nullptr && IsValid(EnemyCharacter))
			{
				EnemyCharacter->SetGridPointer(this);
				if (bDebug)
				{
					UE_LOG(LogTemp, Warning, TEXT("Assigned grid to enemy"));
				}
			}
		}
	}
}

void AGrid::OnDebugDraw() const
{
	// Draw border of grid 
	DrawDebugBox(GetWorld(), GetActorLocation(), FVector(GridSize.X / 2, GridSize.Y / 2, GridSize.Z / 2), FColor::Red, true);

	// draw each node where unwalkable nodes are red and walkable green
	for(int x = 0; x < GridLengthX; x++)
	{
		for(int y = 0; y < GridLengthY; y++)
		{
			for(int z = 0; z < GridLengthZ; z++)
			{
				GridNode* Node = GetNodeFromGrid(x, y, z);
				FColor Color = Node->IsWalkable() ? FColor::Green : FColor::Red;

				if (!Node->IsWalkable())
				{
					DrawDebugBox(GetWorld(), Node->GetWorldCoordinate(), FVector(NodeRadius, NodeRadius, NodeRadius), FColor::Red, true);
				}
			}
		}
	}
}
