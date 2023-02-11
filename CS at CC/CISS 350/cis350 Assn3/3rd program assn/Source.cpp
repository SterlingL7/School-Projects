/*CISS 350 Programming assignment 3
Employee Queue
Sterling LaBarbera

This program is modified from the 2nd assignment. changed the class name to employeeQ to reflect ADT change. 
Now, removing will dequeue the first employee in the list, and add will add a new employee to the back of the list.
Print is still available but not shown to the user.
most of the modifications were made in the class header file.
altered the incorrect error message since it appeared to not function properly before.
It would take input but since the loop iterated again, the input would be overwritten without being used.
Now, it will just print error and go back to the beginning to accept the input.
*/

#include <iostream>
#include <fstream>
#include <string>
#include "employeeQ.h"

using namespace std;

employeeQ EmpQ; //init a new list

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

		EmpQ.addNew(empNum, empNameF, empNameL, YoS);//add a node with the info from file
	}

	EmpQ.PrintList();
	cout << "Welcome to the employee registry." << endl; //give the user a list of acceptable inputs
	cout << "Available options: " << endl;				 //hopefully they use all caps since the options are in all caps
	cout << "ADD/REMOVE, to add or remove an employee." << endl;//also added all lower case support 
	cout << "size, to show the number of employees." << endl;
	//cout << "PRINT, to print the full list of employees" << endl; this is now a secret option for debugging since it isnt specified
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

			EmpQ.addNew(empNum, empNameF, empNameL, YoS); //call the add function of the employeelist class
		}					// if the user inputs one of the listed inputs, 
		else if (action == "REMOVE" || action == "remove")	//the program executes the designated operation
		{
			EmpQ.deQEmp();
		}
		else if (action == "SIZE" || action == "size")//call Length if count input
			cout << "The current number of employees is: " << EmpQ.Length() << endl;
		else if (action == "PRINT" || action == "print")//call PrintList if print is input
			EmpQ.PrintList();
		else if (action == "EXIT" || action == "exit")//exit program
		{
			cout << "Thank you, Have a nice day.";
			cont = false;
		}
		else			//if the user inputs an invalid option, print error msg and get new input.
		{
			cout << "That is not a valid command. Please enter another. Input must be all caps or all lower-case" << endl;
		}
	}
	return 0;
}







