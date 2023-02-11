package chapter7;
/*Sterling LaBarbera
 * Chapter 7
 * Programming Challenge 2
 * 
 *This class keeps track of the payroll data for 7 employees
 *the main program sets all the data at the beginning, then it can be updated when needed
 *
 */
public class Payroll {
	//the important data is initialized below. The given employee IDs and 7 element arrays for their data
	public final int NUM_EMPLOYEES = 7;
	private int[]employeeId = new int[] {5658845,4520125,7895122,8777541,8451277,1302850,7580489};
	private int[]hours = new int[NUM_EMPLOYEES];
	private double[]payRate = new double[NUM_EMPLOYEES];
	private double[]wages = new double[NUM_EMPLOYEES];
	
	public int findEmployee(int empID) {//this method searches the gets the index for the employee ID
											//saves some lines of code, makes it easier to read the other methods
		for(int x = 0; x < NUM_EMPLOYEES; x++) {
			if(employeeId[x] == empID)
				return x;
		}
		return -1;//requires a return value. test program will not enter invalid values
	}
	
	public void setEmpData(int empID, int hrs, double rate) {//set the data for the selected employee
		
		int employee = findEmployee(empID);
		hours[employee] = hrs;
		payRate[employee] = rate;
		wages[employee] = hours[employee]*payRate[employee];
	}
	
	public double getGrossPay(int empID) {//get gross pay for selected employee
		
		int employee = findEmployee(empID);
		return wages[employee];
	}
	
	public void updateHours(int empID, int hrs) {//chage hours for selected employee
		int employee = findEmployee(empID);
		hours[employee] = hrs;
	}
	
	public void updateRate(int empID, double rate) {//change pay rate for selected employee
		int employee = findEmployee(empID);
		payRate[employee] = rate;
	}
	
	public int getHours(int empID) {//get current hours for employee
		int employee = findEmployee(empID);
		return hours[employee];
	}
	
	public double getRate(int empID) {//get current pay rate for employee
		int employee = findEmployee(empID);
		return payRate[employee];
	}
	
}
