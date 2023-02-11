/*assignment 1 receipt printer
  Sterling LaBarbera
  3/11/18

  I used a topdown design approach with an array to hold the struct ADT. Because the amount of data in each element is so small, 
  I decided not to use classes for the objects.
*/
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Inventory //create a structure to hold the inventory data for the array
{
	int invNum;
	string Name;
	float price;
	string tax;
};

int main ()
{
	int pcode;		//entred inventory number
	int amount;		//number of items wanted
	int totamount = 0; //total number of items on receipt
	float runningtot = 0; //total cost on for receipt
	char k = 'y'; //character 
	bool found = false; //used when checking if item is on inventory list
	int invnum = 0;  //variables to hold the read data for the loop
	string invname; 
	float invprice;
	string invtax = "N";
	int c = 0;
	float taxrate = .08;
	float taxtot = 0;

	ifstream inventory; //set the read file
	inventory.open("Invent.dat.txt");
	ofstream receipt("Receipts.out"); //set the write file
	Inventory StoreInv[50];

	do    //read from the input file into the inventory structure
	{
		if (inventory.eof())  //for some reason i had trouble with duplicating the last entry on the input file
			break;			  //so i this is there to stop at end of file instead of doing one more iteration
		inventory >> invnum;	//load info
		inventory >> invname;
		inventory >> invprice;
		inventory >> invtax;
		for (int x = 0; x < c; x++) //loop to check for duplicate item numbers. if it is, go to the next line of the file
		{
			if (invnum == StoreInv[x].invNum)
			{
				receipt << "Item " << invnum << " is a duplicate. Not added." << endl; 
				inventory >> invnum;
				inventory >> invname;
				inventory >> invprice;
				inventory >> invtax;
			}
		}
		cout << invnum << " " << invname << " " << invprice << " " << invtax << endl;
		StoreInv[c].invNum = invnum;
		StoreInv[c].Name = invname;
		StoreInv[c].price = invprice;
		StoreInv[c].tax = invtax;

		receipt << StoreInv[c].invNum << " " << StoreInv[c].Name << " " << StoreInv[c].price << " " << StoreInv[c].tax << " " << endl;

		c++;
		
	} while (!inventory.eof());

	do                 //take inputs from usr, ask if they have more customers after every 0 entry, end if they say no
	{
		cout << "Enter a product code:\n";
		cin >> pcode;
				
		for (int n = 0; n < 50; n++)
		{
			if (pcode == StoreInv[n].invNum) //when the entered item code matches one in the inventory, output its data and add the costs to the total
			{
				cout << StoreInv[n].Name << endl;
				cout << "How many would you like?\n";
				cin >> amount;
				while (amount > 100)
				{
					cout << "That is too many. Please enter a number less than 100." << endl; //make sure they enter appropriate amounts
					cin >> amount;
				}
				receipt << amount << " " << StoreInv[n].invNum << " " << StoreInv[n].Name << " " << StoreInv[n].price*amount << " " << endl;
				runningtot = runningtot + StoreInv[n].price*amount; //add the cost of the items to the sub total
				if (StoreInv[n].tax == "T")
					taxtot = taxtot + taxrate * StoreInv[n].price * amount; //add up the taxable total
				totamount = totamount + amount; //increment the total items being purchased
				found = true;	//note that item is found
			}
		}
		
		if (found == false) //if the item was not found, found stays false
			receipt << "~~~~Item number " << pcode << " not found." << endl;

		if (pcode == 0) //check if they are finished with the current customer order
		{
			receipt << "\n" << "Total: " << totamount << " items " << "@ $" << runningtot << endl; //print pretax total
			receipt << "Taxes: " << taxtot << endl;	//print taxes
			cout << "Total: " << totamount << " items " << "$" << runningtot + taxtot << endl; //output totals for current customer
			runningtot = 0;			//reset the totals for the next customer
			totamount = 0;
			taxtot = 0;
			cout << "would you like to enter another customer?  y/n \n"; //check if they wish to continue with another customer
			cin >> k;
		}
	} while (k == 'y');
	receipt << "Thank you for shopping. Have a nice day!" << endl;
	cout << "Thank you for shopping. Have a nice day!" << endl;
	return(0);
	;
}