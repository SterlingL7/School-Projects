#include <string>
#include <iostream>
using namespace std;

struct employee //make structure for employee data
{
	int emNum;
	string Fname;
	string Lname;
	int Years;
	employee* next;
};

class employeeQ
{
private:
	employee * front;	//front of queue
	employee * back;	//back of queue
	int length;

public:				//functions below are defined in the employeelist.cpp file

	employeeQ(); //default constructor
	~employeeQ(); //default destructor

	void MakeEmpty();
	void PrintList();
	void addNew(int Num, string NameF, string NameL, int Year);
	void deQEmp();
	int Length() const;
};

	employeeQ::employeeQ() //constructor inits global info to 0
	{
		length = 0;
		front = NULL;
		back = NULL;
	}

	employeeQ::~employeeQ()// put this in but wasnt sure when to call it, or if i need to 
	{
		employee* templocation;

		while (front != NULL)
		{
			templocation = front;
			front = front->next;
			delete templocation;
		}
	}



	void employeeQ::addNew(int Num, string NameF, string NameL, int Year)	//function to add employee to list
	{
		employee* newEm;		//allocate space to new employee
		newEm = new employee;
		newEm->emNum = Num;		//put values entered into the new employee's data
		newEm->Fname = NameF;
		newEm->Lname = NameL;
		newEm->Years = Year;
		newEm->next = NULL;
		if (back != NULL)
		{
			back->next = newEm;
		}
		if (front == NULL)
		{
			front = newEm;
		}
		back = newEm;
		length++;				//increase length of list
	}

/*	void employeeQ::deleteEmp(int del)	//function to remove an employee
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
	}   */

	void employeeQ::deQEmp()
	{
		if (front != NULL)
		{
			string F;	//local variables to store the name of deleted employee
			string L;	//used to print the name after deleting node

			employee* temp = front;
			F = front->Fname;
			L = front->Lname;
			front = front->next;
			delete temp;
			length--;
			cout << F << ' ' << L << " has been removed from the queue." << endl;
		}
		else
			cout << "There are no employees in the queue."
	}

	void employeeQ::MakeEmpty() //clear the list starting from the first node
	{
		employee* temPoint;

		while (back != NULL) //continue until the external pointer points to nothing
		{
			temPoint = front; //use temporary pointer to hold location of start while start is pointed to the next node
			front = front->next;
			delete temPoint;	//delete the node at previous location of start 
		}

		length = 0; //reset length to 0 after all elements are deleted
	}

	void employeeQ::PrintList() //seach by number
	{
		employee* temploc = front;
		int loc = 0;

		while (loc < length)// loop through each node and print with ok formatting
		{
			cout << temploc->emNum << ' ';
			cout << temploc->Fname << ' ';
			cout << temploc->Lname << ' ';
			cout << temploc->Years;
			cout << endl;
			loc++;
			temploc = temploc->next;
		}
		cout << endl;//add space between printed list and next text on console
	}

	int employeeQ::Length() const   //show number of employees in list
	{
		return length;
	}




