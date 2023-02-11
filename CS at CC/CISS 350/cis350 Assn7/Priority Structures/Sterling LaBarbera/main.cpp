/*
Sterling LaBarbera
ciss 350 assignment 7

this is the driver program for the 2 priority structures.
generates values and then returns the values as it dequeues each structure
*/

#include <string>
#include <iostream>
#include <time.h>
#include "PriorityQueue.h"
#include "ListPQ.h"

using namespace std;

int main()
{
	const int length = 10;
	int data[length] = {};
	HeapPriorityQ<int> PriorityHeap(length);
	ListPQ<int> PriorityList(length);

	srand(time(NULL));//seed random numbers

	for (int i = 0; i < length; i++)//generates a random value for data array and stores in the heap or list
	{
		data[i] = rand()%100;//generate a random number and place it in heap and list
		PriorityHeap.Enqueue(data[i]);
		PriorityList.Enqueue(data[i]);
	}

	cout << "The original order: " << endl;
	for (int i = 0; i < length; i++)
	{
		cout << data[i] << ' ';
	}
	cout << endl;

	int tempval = -99;
	int queuelen = PriorityHeap.GetLength();

	for (int i = 0; i < length; i++)//dequeue heap and output the values
	{
		PriorityHeap.Dequeue(tempval);
		cout << tempval << ' ';
	}
	cout << endl;
	cout << PriorityHeap.GetDOPS() << " dequeue operations executed." << endl;
	cout << PriorityHeap.GetEOPS() << " enqueue operations executed." << endl;
	cout << "Heap size: " << PriorityHeap.GetLength() << endl;
	system("pause");

	for (int i = 0; i < length; i++)//dequeue list and output values
	{
		PriorityList.Dequeue(tempval);
		cout << tempval << ' ';
	}
	cout << endl;
	cout << PriorityList.GetDOPS() << " dequeue operations executed." << endl;
	cout << PriorityList.GetEOPS() << " enqueue operations executed." << endl;
	cout << "List length: " << PriorityList.GetLength() << endl;
	system("pause");
	return 0;
}