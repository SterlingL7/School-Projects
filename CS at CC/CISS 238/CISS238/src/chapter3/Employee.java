package chapter3;

/*Sterling LaBarbera
  Chapter 3
  Programming Challenge 1
  This is the base employee class file
  can set or get the name, ID #, department, or position of the employee
 */
public class Employee{

	private String name;	//the attributes of the employee
	private String department;
	private String position;
	private int idNumber;

	public void setName(String newName)//set the name
	{
		name = newName;
	}

	public void setDepartment(String newDept)//set the department
	{
		department = newDept;
	}
	
	public void setPosition(String newPos)//set the position
	{
		position = newPos;
	}
	
	public void setIdNumber(int newID)//set the ID #
	{
		idNumber = newID;
	}
	
	//the funtions to display each attribute:
	public String getName()
	{
		return name;
	}
	
	public String getDepartment()
	{
		return department;
	}
	
	public String getPosition()
	{
		return position;
	}
	
	public int getIdNumber()
	{
		return idNumber;
	}
}
