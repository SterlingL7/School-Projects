/*CISS 350 Programming assignment 2
Employee list
Sterling LaBarbera

This program uses a singly linked list from employeelist.h to keep an employee registry.
the input is from a text file and the only output is to the console.
*/

#include <iostream>
#include <fstream>
#include <string>
#include "employeelist.h"

using namespace std;

employeelist EmpList; //init a new list

int main()
{
	bool cont = true;	//variable used to check for exit command and break loop
	string action;		//variable for inputs
	int empNum = 0;			//employee number
	string empNameF;	//first name
	string empNameL;	//last name
	int YoS;		//Years of Service
					//load Employee.txt file
	ifstream employees; //set the read file
	employees.open("Employee.txt");//load file

	while (!employees.eof())//read file into the list, each loop should be 1 employee from list
	{
		employees >> empNum;
		employees >> empNameF;
		employees >> empNameL;
		employees >> YoS;

		EmpList.addNew(empNum, empNameF, empNameL, YoS);//add a node with the info from file
	}


	cout << "Welcome to the employee registry." << endl; //give the user a list of acceptable inputs
	cout << "Available options: " << endl;				 //hopefully they use all caps since the options are in all caps
	cout << "ADD/REMOVE, to add or remove an employee." << endl;//also added all lower case support 
	cout << "COUNT, to show the number of employees." << endl;
	cout << "PRINT, to print the full list of employees" << endl;
	cout << "EXIT, to quit." << endl;
	
	while (cont)//cont = false when the input is exit
	{
		cout << "What would you like to do?" << endl;
		cin >> action;		//get the input from user
		cout << endl;

		if (action == "ADD" || action == "add")	//multi part if statement to check the input
		{
			cout << "Please enter the name of the new employee, first then last:" << endl;
			cin >> empNameF >> empNameL;
			cout << endl;

			cout << "Please enter the employee number for " << empNameF << " " << empNameL << ": " << endl;
			cin >> empNum;
			cout << endl << "Please enter their years of service: " << endl;
			cin >> YoS;

			EmpList.addNew(empNum, empNameF, empNameL, YoS); //call the add function of the employeelist class
		}					// if the user inputs one of the listed inputs, 
		else if (action == "REMOVE" || action == "remove")	//the program executes the designated operation
		{
			cout << "Enter the number of the employee you would like to remove: " << endl;
			cin >> empNum;
			cout << endl;

			EmpList.deleteEmp(empNum);
		}
		else if (action == "COUNT" || action == "count")//call Length if count input
			cout << "The current number of employees is: " << EmpList.Length() << endl;
		else if (action == "PRINT" || action == "print")//call PrintList if print is input
			EmpList.PrintList();
		else if (action == "EXIT" || action == "exit")//exit program
		{
			cout << "Thank you, Have a nice day.";
			cont = false;
		}
		else			//if the user inputs an invalid option, print error msg and get new input.
		{
			cout << "That is not a valid command. Please enter another. Input must be all caps or all lower-case" << endl;
			cin >> action;
			cout << endl;
		}
	}
	return 0;
}







