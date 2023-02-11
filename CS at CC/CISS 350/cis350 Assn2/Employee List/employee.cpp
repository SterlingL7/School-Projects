#include <string>

class employee
{
private:
	int emNum;
	string Fname;
	string Lname;
	int Years;
	struct Employee *nextEmp;

public:
	employee();// Default constructor

	void numInit(int); //define the data for employee
	void nameInit(string, string);
	void yearsInit(int);

	int getNum();//access info of employee
	string getName();
	int getYears();
}