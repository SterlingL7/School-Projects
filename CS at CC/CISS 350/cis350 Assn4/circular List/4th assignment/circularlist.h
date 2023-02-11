/*Sterling LaBarbera 
CISS 350 project 4
this header file contains implementation for the circular linked list.
it supports any input for which '<' and '==' are defined
*/

#include <string>
#include <iostream>
using namespace std;

template <class ItemType>
struct NodeType;

/* Assumption:  ItemType is a type for which the operators
"<" and "==" are defined—either an appropriate built-in type or a class that overloads these operators. */

template <class ItemType>
class UnsortedType
{
public:
	// Class constructor, destructor, and copy constructor
	UnsortedType();
	UnsortedType(const UnsortedType<ItemType>&);
	~UnsortedType();

	//void FindItem(NodeType<ItemType>* listData, ItemType item, NodeType<ItemType>*& location, NodeType<ItemType>*& predLoc, bool& found);

	void operator=(UnsortedType<ItemType>); //overload assignment operator since it will be used often

	bool IsFull() const;//check if list is full

	int  GetLength() const;//print the current number of items in list

	void RetrieveItem(ItemType& item, bool& found); //search for a specific item
	
	void InsertItem(ItemType item); //add item to list between first and last node
	
	void DeleteItem(ItemType item); //remove specified item
	
	void ResetList(); //sets the current location to the first node
	
	void GetNextItem(ItemType&); //gets the item after current location
	
	void printList();
private:
	NodeType<ItemType>* listData; //pointer to first node
	int length;			//list length
	NodeType<ItemType>* currentPos;	//points at the most recent node
};

template<class ItemType>	//implementation of a finder function as in the text.
void FindItem(NodeType<ItemType>* listData, ItemType item, NodeType<ItemType>*& location, NodeType<ItemType>*& prev, bool& found)
{
	bool searching = true;
	location = listData->next;
	prev = listData;
	found = false;

	while (searching && !found)
	{
		if (item == location->data)
			found = true;
		else
		{
			prev = location;
			location = location->next;
			searching = (location != listData->next);
		}
	}
}

template <class ItemType>
struct NodeType
{
	ItemType data;
	NodeType* next;
};


template<class ItemType> //regular constructor
UnsortedType<ItemType>::UnsortedType()
{
	length = 0;
	listData = NULL;
	currentPos = listData;
}

template<class ItemType>
UnsortedType<ItemType>::UnsortedType(const UnsortedType<ItemType>& list1)// copy constructor
{
	NodeType<ItemType>* list;	//creates a new list
	NodeType<ItemType>* list2;

	if (list1 != NULL)//makes sure the parameter list isnt empty
	{
		while (list1->next != listData)//makes a copy of each node 
		{
			list2->data = list1->data;
			list1 = list1->next;
		}
	}
	else

}

template<class ItemType>
UnsortedType<ItemType>::~UnsortedType()
{
	NodeType<ItemType>* temp;
	while (currentPos->next != NULL)
	{
		temp = currentPos->next;
		delete currentPos;
		currentPos = temp;
	}
	delete currentPos;
}

template<class ItemType>
int  UnsortedType<ItemType>::GetLength() const
{
	return length;
}

template<class ItemType>
bool UnsortedType<ItemType>::IsFull() const
{
	return(false)
}

template<class ItemType>
void UnsortedType<ItemType>::operator=(UnsortedType<ItemType> anotherList) //overload assignment operator
{
	//item->data = ItemType->data;
}

template<class ItemType>
void UnsortedType<ItemType>::InsertItem(ItemType item)
{
	NodeType<ItemType>* newNode;
	NodeType<ItemType>* previous;
	NodeType<ItemType>* current;

	newNode = new NodeType<ItemType>;
	newNode->data = item;

	bool found;
	if (listData == NULL)
	{
		listData = newNode;
		newNode->next = newNode;
		currentPos = listData;
	}
	else
	{
		while (currentPos->next != listData)
		{
			currentPos = currentPos->next;
		}
		newNode->next = listData;
		currentPos->next = newNode;
	}
	length++;
	/*
	if (IsFull())
		cout << "Cannot add another item to the list. List is full." << endl;
	else
	{
		while (*current->next.data != listData && item != current->data)
		{
			previous = current;
			current = current->next;
		}
		if (item == current)
		{
			cout << "That item is already in the list." << endl;
		}
		else
		{
			newNode->next = current;
			previous->next = newNode;
		}
	}*/
}

template<class ItemType>
void UnsortedType<ItemType>::DeleteItem(ItemType item)
{
	NodeType<ItemType>* current;
	NodeType<ItemType>* previous;
	bool found;

	FindItem(listData, item, current, previous, found);

	if (previous == current)
		listData = NULL;
	else
	{
		previous->next = current->next;
		if (current == listData)
			listData = previous;
	}
	delete current;
	length--;
}

template<class ItemType>
void UnsortedType<ItemType>::RetrieveItem(ItemType& item, bool& found)
{
	found = false;
	NodeType<ItemType>* current = currentPos;
	while (current->next != currentPos && item != current)
	{
		current = current->next;
	}
	if (item == current)
	{
		found = true;
	}
	else
	{
		cout << "Could not find specified item." << endl;
	}
	currentPos = current;
}

template<class ItemType>
void UnsortedType<ItemType>::GetNextItem(ItemType& current)
{
	currentPos = current->next;
}

template<class ItemType>
void UnsortedType<ItemType>::ResetList()
{
	currentPos = listData;
}

template<class ItemType>
void UnsortedType<ItemType>::printList()
{
	NodeType<ItemType>* current = listData;
	while (current->next != listData)
	{
		cout << current->data << endl;
		current = current->next;
	}
}