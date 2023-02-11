#include <string>
#include <iostream>
using namespace std;

struct employee //make structure for employee data
{
	int emNum;
	string Fname;
	string Lname;
	int Years;
	employee * lesser;		//left child
	employee * greater;		//right child
};

class employeeTree
{
private:
	employee * root;		//first node pointer
	int size;
	
public:				
	employeeTree(); //default constructor
	~employeeTree(); //default destructor

	employee* findParent(employee * root, int numfind, employee*& empPtr, employee*& parentPtr);
	void PrintTree(employee * tree);
	void addNew(int Num, string NameF, string NameL, int Year);
	void deleteEmp(int del);
	int Size() const;
	void Insert(employee*& tree, employee emp);
	void Destroy(employee*& tree);
	void deleteNode(employee*& delemp);
	void Print();
	void printRoot();
};

	employeeTree::employeeTree() //constructor inits global info to 0
	{
		size = 0;
		root = NULL;
	}

	employeeTree::~employeeTree()//Deconstructor
	{
		Destroy(root);
	}

	void employeeTree::Destroy(employee*& tree)//recursive delete function used by the deconstructor
	{											//destroys starting from the leaf nodes to the root
		if (tree != NULL)
		{
			Destroy(tree->lesser);
			Destroy(tree->greater);
			delete tree;
		}
	}

	employee* employeeTree::findParent(employee * root, int numfind, employee*& current, employee*& parent)// search for employee, employee number is key value
	{
		bool found = false;

		while (current != NULL && !found)//search until node is found or past the leaf
		{
			if (numfind < current->emNum)
			{
				parent = current;
				current = current->lesser;
			}
			else if (numfind > current->emNum)
			{
				parent = current;
				current = current->greater;
			}
			else
				found = true;
		}
		return parent;
	}

	void employeeTree::Insert(employee*& root, employee emp)//recursive insertion function
	{
		employee* parent;
		
		if (root == NULL)
		{
			root = new employee;
			root->lesser = NULL;
			root->greater = NULL;
			*root = emp;
		}
		else if (emp.emNum < root->emNum)
			Insert(root->lesser, emp);
		else
			Insert(root->greater, emp);
	}

	void employeeTree::addNew(int Num, string NameF, string NameL, int Year)	//function to add employee to list
	{
		employee* newEm;		//allocate space to new employee
		newEm = new employee;
		newEm->emNum = Num;		//put values entered into the new employee's data
		newEm->Fname = NameF;
		newEm->Lname = NameL;
		newEm->Years = Year;
		newEm->lesser = NULL;
		newEm->greater = NULL;

		Insert(root, *newEm);//call insert with for the new employee
		size++;				//increase length of list
	}

	void employeeTree::deleteNode(employee*& delemp)//deletes the specified employee node
	{		
		employee * temp = delemp;

		if (delemp->lesser == NULL)//case for only a right child
		{ 
			delemp = delemp->greater;
			delete temp;
		}
		else if (delemp->greater == NULL)//case for only a left child
		{
			delemp = delemp->lesser;
			delete temp;
		}
		else//case for 2 child nodes
		{
			temp = temp->lesser;
			while (temp->greater != NULL)
				temp = temp->greater;
			delemp->emNum = temp->emNum;
			delemp->Fname = temp->Fname;
			delemp->Lname = temp->Lname;
			delemp->Years = temp->Years;
			delete temp;
		}
	}

	void employeeTree::deleteEmp(int del)	//function to remove an employee
	{
		employee* current;		//location starts at first node
		employee* parent = NULL;

		parent = findParent(root, del, root, parent);

		if (parent != NULL)
		{

			if (parent->emNum > del)
				current = parent->lesser;
			else
				current = parent->greater;


			if (current == root)
				deleteNode(root);
			else
			{
				if (parent->lesser == current)
					deleteNode(parent->lesser);
				else
					deleteNode(parent->greater);
			}
		}
		else
			deleteNode(root);
		/*while (searching)
		{
			if (del < location->emNum)
			{
				parent = location;
				location = location->lesser;
			}
			else if (del > location->emNum)
			{
				parent = location;
				location = location->greater;
			}
			else
			{
				if (location->greater == NULL && location->lesser == NULL)
					location = NULL;
				else if (location->greater == NULL && location->lesser != NULL)
				{
					if (parent->greater == location)
						parent->greater = location->lesser;
					else
						parent->lesser = location->lesser;
				}
				else if (location->greater != NULL && location->lesser == NULL)
				{
					if (parent->greater == location)
						parent->greater = location->greater;
					else
						parent->lesser = location->greater;
				}
				else
				{
					employee* temploc = location->lesser;

					while (temploc->greater != NULL)
					{
						temploc = temploc->greater;
					}
					location->emNum = temploc->emNum;
					location->Fname = temploc->Fname;
					location->Lname = temploc->Lname;
					location->Years = temploc->Years;

					delete &temploc;
				}
			}
		}*/
		size--;
	}

	void employeeTree::PrintTree(employee * tree) //print the whole tree
	{
		if (tree != NULL)
		{
			if(tree->lesser != NULL)
				PrintTree(tree->lesser); //recursively prints the left side, then the right side
			cout << tree->emNum << ' ' << tree->Fname << ' ' << tree->Lname << ' ' << tree->Years << endl;
			if(tree->greater != NULL)
				PrintTree(tree->greater);
		}
	}

	void employeeTree::Print()
	{
		cout << "List of current employees: " << endl;
		PrintTree(root);
		cout << endl;//add space between printed list and next text on console
	}

	int employeeTree::Size() const   //show number of employees in tree
	{
		return size;
	}

	void employeeTree::printRoot()
	{
		cout << root->emNum;
	}


