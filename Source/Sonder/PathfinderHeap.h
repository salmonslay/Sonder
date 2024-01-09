#pragma once

#include "CoreMinimal.h"
#include "GridNode.h"

namespace NodeComparisonFunctions
{
	// Comparison functions for FlockingNode class

	inline bool IsGreaterThan(const GridNode& NodeA, const GridNode& NodeB)
	{
		if (NodeA.FCost == NodeB.FCost)
		{
			return NodeA.HCost > NodeB.HCost;
		}
		return NodeA.FCost > NodeB.FCost;
	}

	inline bool IsLessThan(const GridNode& NodeA, const GridNode& NodeB)
	{
		if (NodeA.FCost == NodeB.FCost)
		{
			return NodeA.HCost < NodeB.HCost;
		}
		return NodeA.FCost < NodeB.FCost;
	}

	inline bool IsEqualTo(const GridNode& NodeA, const GridNode& NodeB)
	{
		return NodeA.FCost == NodeB.FCost && NodeA.HCost == NodeB.HCost;
	}
}


class SONDER_API PathfinderHeap
{

	//FlockingNode* Items;
	TArray<GridNode*> Items;
	int CurrentItemCount = 0;
	

	void SortDown(GridNode* Item)
	{
		while (true)
		{
			const int ChildIndexLeft = Item->HeapIndex * 2 + 1;
			const int ChildIndexRight = Item->HeapIndex * 2 + 2;

			if (ChildIndexLeft < CurrentItemCount)
			{
				int SwapIndex = ChildIndexLeft;

				if (ChildIndexRight < CurrentItemCount)
				{
					if (NodeComparisonFunctions::IsGreaterThan(*Items[ChildIndexLeft],*Items[ChildIndexRight])) SwapIndex = ChildIndexRight;
				}

                if (NodeComparisonFunctions::IsGreaterThan(*Item,*Items[SwapIndex]))
				{
                	ensure(Items.IsValidIndex(SwapIndex));
					Swap(Item, Items[SwapIndex]);
				}
				else {
					return;
				}
			}
			else
			{
				return;
			}
		}
	}

	void SortUp(GridNode* Item)
	{
		int ParentIndex = (Item->HeapIndex - 1) / 2;

		ensure(Items.IsValidIndex(ParentIndex));
		while (ParentIndex >= 0)
		{
			GridNode* ParentItem = Items[ParentIndex];
			ensure(ParentItem != nullptr);
            if (NodeComparisonFunctions::IsLessThan(*Item, *ParentItem))
			{
				Swap(Item, ParentItem);
			}
			else
			{
				break;
			}
			ParentIndex = (Item->HeapIndex - 1) / 2;
			//ensure(Items.IsValidIndex(ParentIndex));
		}
	}

	void Swap(GridNode* ItemA, GridNode* ItemB)
	{
		ensure(ItemA != nullptr);
		ensure(ItemB != nullptr);
		Items.Swap(ItemA->HeapIndex, ItemB->HeapIndex);
		//std::swap(Items[ItemA->HeapIndex], Items[ItemB->HeapIndex]);
		const int ItemAIndex = ItemA->HeapIndex;
		ItemA->HeapIndex = ItemB->HeapIndex;
		ItemB->HeapIndex = ItemAIndex;
	}

public:

	int MaxHeapSize = 0;

	/** Empty constructor*/
	PathfinderHeap(){}

	/*
	virtual ~FlockingHeap()
	{
		delete [] Items;
	}
	*/
	
	PathfinderHeap(int MHeapSize)
	{
		MaxHeapSize = MHeapSize;
		Items.SetNum(MaxHeapSize);
		ensure(Items.Num() == MaxHeapSize);
		CurrentItemCount = 0;
	}

	void Add(GridNode* Item)
	{
		Item->HeapIndex = CurrentItemCount;
		ensure(Items.IsValidIndex(Item->HeapIndex));
		Items[CurrentItemCount] = Item;
		SortUp(Item);
		CurrentItemCount++;
	}
	

	GridNode* RemoveFirst()
	{
		GridNode* FirstItem = Items[0];
		CurrentItemCount--;
		Items[0] = Items[CurrentItemCount];
		Items[0]->HeapIndex = 0;
		SortDown(Items[0]);
		return FirstItem;
	}

	void UpdateItem(GridNode* Item)
	{
		SortUp(Item);
	}

	int Count() const
	{
		return CurrentItemCount;
	}

	bool Contains(const GridNode* Item) 
	{
		ensure(Item != nullptr);
		if(Item->HeapIndex == -1) return false;
		ensure(Items.IsValidIndex(Item->HeapIndex));
		//ensure(Items[Item->HeapIndex] != nullptr);
		return Items[Item->HeapIndex] != nullptr && NodeComparisonFunctions::IsEqualTo(*Items[Item->HeapIndex], *Item);
	}
	
};
