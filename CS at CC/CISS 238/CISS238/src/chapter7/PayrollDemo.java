package chapter7;

/*Sterling LaBarbera
 * Chapter 7
 * Programming Challenge 2
 * 
 * this is the main program for updating and showing employee data
 * gets all the payroll data for each employee in a loop
 * then prints out the gross pay for each employee
 */

import java.util.Scanner;

public class PayrollDemo {

	public static void main(String[] args) {
		
		Payroll A = new Payroll();//initialize the class
		Scanner keyboard = new Scanner(System.in);
		
		int[]employeeId = new int[] {5658845,4520125,7895122,8777541,8451277,1302850,7580489};//user not required to enter empID number
		int hours;//variables for the input data
		double rate;
		
		for(int x = 0; x < A.NUM_EMPLOYEES; x++) {
			//loops through each employee and sets the values
			 
			System.out.println("Enter the hours worked by employee number " + employeeId[x] + ": ");
			hours = keyboard.nextInt();//get the hours for current employee
			
			while(true) {//loop until a valid hour amount is entered
				
				if(hours < 0) {//check if hours is a non negative integer amount. user should not enter non integer values
					System.out.println("ERROR: Enter 0 or greater for hours: ");
					hours = keyboard.nextInt();
				}
				else
					break;
			}
			
			System.out.println("Enter the hourly pay rate for employee number " + employeeId[x] + ": ");
			rate = keyboard.nextDouble();//get the pay rate for current employee
			
			while(true) {//loop until a valid pay rate is entered
				
				if(rate < 6.00) {//check if above the minimum pay rate
					System.out.println("ERROR: Enter 6.00 or greater for pay rate: ");
					rate = keyboard.nextDouble();
				}
				else
					break;
			}
			
			A.setEmpData(employeeId[x], hours, rate);//set the data for the employee
		}
		
		System.out.println("PAYROLL DATA\n============\n");//display the payroll data for all employees
			for (int x = 0; x < A.NUM_EMPLOYEES; x++) {
				System.out.println("Employee ID: " + employeeId[x] +
				"\nGross Pay: $" + A.getGrossPay(employeeId[x]) + "\n");
			}	
	}

}
