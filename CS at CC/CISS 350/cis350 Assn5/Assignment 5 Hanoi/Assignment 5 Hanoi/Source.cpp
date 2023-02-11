/* Assignment 5 Towers of Hanoi
Sterling LaBarbera

This program is a recursive solver for a 3 peg configuration of towers of hanoi, 
but only works from the default starting position.
the minimum moves required is always 2^n-1 where n is the number of discs
*/

#include <string>
#include <iostream>
using namespace std;

void moveDisc(int source,int dest,int extra,int size);

int main()
{
	int size;//# of discs to be solved
	int source = 1; //the 3 pegs are represented by integers for ease of use
	int dest = 2;
	int extra = 3;
	bool cont = true;//used to maintain the program as long as user would like
	char yesno;

	while(cont)
	{ 
		cout << "Towers of Hanoi solver." << endl;
		cout << "Peg one is the starting peg." << endl;
		cout << "Peg two is the destination peg." << endl;
		cout << "Peg three is the extra peg." << endl << endl;
		cout << "How many discs are there?" << endl;
		cin >> size;
		if (size < 0) //check that the number of discs is not negative
			cout << "There must be a non negative number of discs to solve." << endl;
		else
			moveDisc(source, dest, extra, size);	//initial call of the function
		cout << "try a new number of discs? y/n" << endl;	//allow user to use program more than 1 time
		cin >> yesno;										//asks for y or n, but does not do anything unless n is input

		if (yesno == 'n' || yesno == 'N')//check for quit condition
			cont = false;
	}
	system("pause");	//let the user see output by pausing until a key is pressed
	return 0;
}

void moveDisc(int source, int dest, int extra, int size)// this recursive function starts from the largest disc
{												//and calls itself for each disc that must be moved before the current disc
	if (size == 1)		//base case that will be reached for any number of discs
	{
		cout << "Moved disc " << size << " to peg " << dest << endl;
		return;
	}
	else if (size == 0)	//special case that needs to be addressed to prevent infinite negative recursion
		cout << "Did not move any discs because there were none." << endl;
	else
	{
		moveDisc(source, extra, dest, size-1);	//since it must move the smallest disc first
		cout << "Moved disc " << size << " to peg " << dest << endl;
		moveDisc(extra, dest, source, size - 1);	//since it needs to move all the other discs onto the largest one
	}
}