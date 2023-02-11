/*
Sterling LaBarbera
ciss 350 assignment 7

this file has the implementation of the min heap and the priority queue
the heap is implemented using an array based binary tree
the queue is a linked list
*/
#pragma once
#include <string>
#include <iostream>
using namespace std;

template<class ItemType>

struct MinHeap
{
	int ReheapUP(int root, int bottom);
	int ReheapDN(int root, int bottom);
	void Swap(ItemType& parent, ItemType& child);
	ItemType* elements;
	int numElements;
};

template<class ItemType>
int MinHeap<ItemType>::ReheapDN(int root, int bottom)//restores heap property after node is removed
{
	int minChild;
	int rightChild;
	int leftChild;
	int totaccessed = 0;

	leftChild = root * 2 + 1;
	rightChild = root * 2 + 2;
	if (elements[leftChild] >= elements[bottom])
	{
		if (elements[leftChild] == elements[bottom])
		{
			minChild = leftChild;
			totaccessed++;
		}
		else
		{
			if (elements[leftChild] <= elements[rightChild])
			{
				minChild = leftChild;
				totaccessed++;
			}
			else
			{
				minChild = rightChild;
				totaccessed++;
			}
		}
		if (elements[root] > elements[minChild])
		{
			Swap(elements[root], elements[minChild]);
			totaccessed += 3;
			totaccessed += ReheapDN(minChild, bottom);
		}
	}
	return totaccessed;
}

template<class ItemType>
int MinHeap<ItemType>::ReheapUP(int root, int bottom)//restores heap property when a node is inserted
{
	int parent;
	int totalaccessed = 0;

	if (elements[bottom] < elements[root])
	{
		parent = (bottom - 1) / 2;
		totalaccessed++;
		if (elements[parent] > elements[bottom])
		{
			Swap(elements[parent], elements[bottom]);
			totalaccessed += 3;
			totalaccessed += ReheapUP(root, parent);
		}
	}
	return totalaccessed;
}

template<class ItemType>
void MinHeap<ItemType>::Swap(ItemType& parent, ItemType& child)//swaps two noded in the heap
{
	ItemType temp;
	temp = elements[parent];
	elements[parent] = elements[child];
	elements[child] = temp;
}

template<class ItemType>
class HeapPriorityQ
{
public:
	HeapPriorityQ(int);

	~HeapPriorityQ();
	void MakeEmpty();
	bool IsEmpty() const;
	bool IsFull() const;
	void Enqueue(ItemType newItem);
	void Dequeue(ItemType& item);
	int GetLength();
	int GetDOPS();
	int GetEOPS();
	void resetOPS();
private:
	int length;
	MinHeap<ItemType> items;
	int maxItems;
	int enQops;
	int deQops;
};

template<class ItemType>
HeapPriorityQ<ItemType>::HeapPriorityQ(int max)//constructor
{
	maxItems = max;
	items.elements = new ItemType[max];
	length = 0;
	enQops = 0;
	deQops = 0;
}

template<class ItemType>
void HeapPriorityQ<ItemType>::MakeEmpty()
{
	length = 0;
}

template<class ItemType>
HeapPriorityQ<ItemType>::~HeapPriorityQ()//destructor
{
	delete[] items.elements;
}

template<class ItemType>
void HeapPriorityQ<ItemType>::Dequeue(ItemType& item)//removes item from the heap
{
	if (length == 0)
		cout << "there are no items to remove" << endl;
	else
	{
		item = items.elements[0];
		items.elements[0] = items.elements[length - 1];
		deQops++;
		length--;
		deQops += items.ReheapDN(0, length - 1);
	}
}

template<class ItemType>
void HeapPriorityQ<ItemType>::Enqueue(ItemType newItem)
{
	if (length == maxItems)
		cout << "The list is full, could not add a new item." << endl;
	else
	{
		length++;
		items.elements[length - 1] = newItem;
		enQops++;
		enQops += items.ReheapUP(0,length - 1);
	}
}

template<class ItemType>
bool HeapPriorityQ<ItemType>::IsFull() const
{
	return length == maxItems;
}

template<class ItemType>
bool HeapPriorityQ<ItemType>::IsEmpty() const
{
	return length == 0;
}

template<class ItemType>
int HeapPriorityQ<ItemType>::GetLength()
{
	return length;
}

template<class ItemType>
int HeapPriorityQ<ItemType>::GetEOPS()
{
	return enQops;
}

template<class ItemType>
int HeapPriorityQ<ItemType>::GetDOPS()
{
	return deQops;
}

template<class ItemType>
void HeapPriorityQ<ItemType>::resetOPS()
{
	enQops = 0;
	deQops = 0;
}