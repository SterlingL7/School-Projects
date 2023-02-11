/*Sterling LaBarbera
CISS 350 assignment 4
driver program

this program implements the template specified in the assignment
from the header file
*/

#include <iostream>
#include <string>
#include <fstream>
#include "circularlist.h"
using namespace std;

UnsortedType<float> CList;	//initialize an unsortedtype float

int main()
{
	bool cont = true;//changed when time to exit
	float data;
	string input;	//user input variable
	ifstream Items; //set the read file
	Items.open("Items.txt");//load file

	while (!Items.eof())//read file into the list, each loop should be one item
	{
		Items >> data;

		CList.InsertItem(data);//add a node with the info from file
	}

	cout << "This is a circular list." << endl;
	cout << "You may add to it or remove from it." << endl;
	cout << " acceptable inputs: add, delete, size, exit." << endl;

	CList.printList();

	while (cont)
	{
		cin >> input;
		cout << endl;

		if (input == "exit" || input == "EXIT")
		{
			cout << "Now exiting." << endl;
			cont = false;
		}
		else if (input == "add" || input == "ADD")
		{
			float newitem;
			cout << "Enter a value to add to the list" << endl;
			cin >> newitem;
			cout << endl;
			CList.InsertItem(newitem);
		}
		else if (input == "delete" || input == "DELETE")
		{
			float newitem;
			cout << "Enter a value to remove from the list" << endl;
			cin >> newitem;
			cout << endl;
			CList.DeleteItem(newitem);
		}
		else if (input == "size" || input == "SIZE")
			cout << "There are " << CList.GetLength() << " items in the list." << endl;
		else if (input == "print")
			CList.printList();
		else
			cout << "That is invalid" << endl;
	}
	return 0;
}