#pragma once
//Sterling LaBarbera
//ciss350 assignment 7

//this header contains the linked list implementation of the priority queue

#include <string>
#include <iostream>

using namespace std;

template<class ItemType>

struct PList
{
	ItemType* elements;
	int numElements;
};

template<class ItemType>
class ListPQ
{
public:
	ListPQ(int max);
	~ListPQ();
	void Enqueue(ItemType newitem);
	void Dequeue(ItemType& item);
	int GetLength();
	int GetEOPS();
	int GetDOPS();
	void ResetOPS();

private:
	int length;
	PList<ItemType> items;
	int enQops;
	int deQops;
	int maxsize;
};

template<class ItemType>
ListPQ<ItemType>::ListPQ(int max)
{
	length = 0;
	items.elements = new ItemType[max];
	enQops = 0;
	deQops = 0;
	maxsize = max;
}

template<class ItemType>
ListPQ<ItemType>::~ListPQ()
{
		delete [] items.elements;
}

template<class ItemType>
void ListPQ<ItemType>::Enqueue(ItemType newitem)
{
	
	if (length == maxsize)
		cout << "Cannot add to list." << endl;
	else if (length == 0)
	{
		length++;
		items.elements[length - 1] = newitem;
		enQops++;
	}
	else
	{
		
		int a = length;
		items.elements[length] = newitem;
		enQops++;
		int temp;

		while (items.elements[a] < items.elements[a - 1] && a >= 0 )
		{	
			if (items.elements[a - 1] == -1 && a >= 0)
			{
				enQops++;
				a--;
			}
			else
			{
				temp = items.elements[a];
				items.elements[a] = items.elements[a - 1];
				items.elements[a - 1] = temp;
				enQops += 3;
				a--;
			}
		}
		length++;
	}
}

template<class ItemType>
void ListPQ<ItemType>::Dequeue(ItemType& item)
{
	length--;
	int x = 0;
	while (items.elements[x] == -1)
	{
		x++;
	}
	item = items.elements[x];
	items.elements[x] = -1;
	deQops++;
}

template<class ItemType>
int ListPQ<ItemType>::GetLength()
{
	return length;
}

template<class ItemType>
int ListPQ<ItemType>::GetEOPS()
{
	return enQops;
}

template<class ItemType>
int ListPQ<ItemType>::GetDOPS()
{
	return deQops;
}

template<class ItemType>
void ListPQ<ItemType>::ResetOPS()
{
	enQops = 0;
	deQops = 0;
}