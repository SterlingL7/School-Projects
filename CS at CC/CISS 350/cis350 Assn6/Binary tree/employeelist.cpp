#include "employeelist.h"
#include <iostream>
#include <string>



employeelist::employeelist() //constructor inits global info to 0
{
	length = 0;
	start = NULL;
}

employeelist::~employeelist()
{
	employee* templocation;

	while (start != NULL)
	{
		templocation = start;
		start = start->next;
		delete templocation;
	}
}

	void employeelist::addNew(int Num, string NameF, string NameL, int Year)	//function to add employee to list
	{
		employee* newEm;		//allocate space to new employee
		newEm = new employee;	
		newEm->emNum = Num;		//put values entered into the new employee's data
		newEm->Fname = NameF;
		newEm->Lname = NameL;
		newEm->Years = Year;
		newEm->next = start;
		start = newEm;
		length++;				//increase length of list
	}

	void employeelist::deleteEmp(int del)	//function to remove an employee
	{
		bool moretoSearch;
		employee* location = start;		//location starts at first node
		employee* templocation;		//temp pointer for traversing list

		moretoSearch = (location != NULL);

		if (del == start->emNum)	//see if first node is the one to be deleted and delete it if yes
		{
			templocation = location;
			start = start->next;
		}
		else//not the first node? loop to check each node 
		{
			while (del != (location->next)->emNum)	//comparing input to the number at the next node.
			{
				location = location->next; //move one node
			}
			//when it finds the node with input number,
			//that pointer is put in the temp pointer and the pointer to next node is updated to point at the one after

			templocation = location->next; //increment the temporary pointer
			location->next = (location->next)->next; //update the next item location
		}
		delete templocation; //delete the node that matched
		length--;	
	}

	void employeelist::MakeEmpty() //clear the list starting from the first node
	{
		employee* temPoint;

		while (start != NULL) //continue until the external pointer points to nothing
		{
			temPoint = start; //use temporary pointer to hold location of start while start is pointed to the next node
			start = start->next;
			delete temPoint;	//delete the node at previous location of start 
		}
		
		length = 0; //reset length to 0 after all elements are deleted
	}

	void employeelist::PrintList() //seach by number
	{
		employee* temploc = start;
		bool moretoSearch;
		moretoSearch = (temploc->next != NULL)
		
		while (moretosearch)
		{
			cout << temploc->emNum << ' ';
			cout << temploc->Fname << ' ';
			cout << temploc->Lname << ' ';
			cout << temploc->Year;
			cout << endl;
		}
	}
	
	int employeelist::Length() const   //show number of employees in list
	{
		return length;
	}
