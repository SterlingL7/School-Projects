package chapter6;
/*Sterling LaBarbera
 * Chapter 6
 * Programming Challenge 5
 * 
 * This is a month class that allow the user to enter the number 
 * or the name of the month to generate a correct month
 * It also has a default constructor that sets the month to January
 * month names are stored in a string array for easier usage of month number
 */
public class Month {

	private int monthNumber;//value of month
	//monthNames holds all the names of months
	private String[] monthNames = new String[] { "January","February","March","April","May","June",			
												"July","August","September","October","November","December" };
	
	public Month() {//default
		monthNumber = 1;
	}
	
	public Month(int num) {//user defined month value
		monthNumber = num;
	}
	
	public Month(String monthName){/*this is a non case sensitive way to set month by name it is on 
									*the calling program to check for valid names, otherwise this will 
									*produce a default value of january like the default constructor
									*/
			if(monthName.equalsIgnoreCase(monthNames[0]))
				monthNumber = 1;
			else if(monthName.equalsIgnoreCase(monthNames[1]))
				monthNumber = 2;
			else if(monthName.equalsIgnoreCase(monthNames[2]))
				monthNumber = 3;
			else if(monthName.equalsIgnoreCase(monthNames[3]))
				monthNumber = 4;			
			else if(monthName.equalsIgnoreCase(monthNames[4]))
				monthNumber = 5;
			else if(monthName.equalsIgnoreCase(monthNames[5]))
				monthNumber = 6;
			else if(monthName.equalsIgnoreCase(monthNames[6]))
				monthNumber = 7;
			else if(monthName.equalsIgnoreCase(monthNames[7]))
				monthNumber = 8;
			else if(monthName.equalsIgnoreCase(monthNames[8]))
				monthNumber = 9;
			else if(monthName.equalsIgnoreCase(monthNames[9]))
				monthNumber = 10;
			else if(monthName.equalsIgnoreCase(monthNames[10]))
				monthNumber = 11;
			else if(monthName.equalsIgnoreCase(monthNames[11]))
				monthNumber = 12;
			else
				monthNumber = 1;
	}
	
	public void setMonthNumber(int moNum) {//change the month number

		if(moNum > 0 && moNum < 13)
			monthNumber = moNum;
		else 
			monthNumber = 1;
	}
	
	public int getMonthNumber() {//get the month number
		return monthNumber;
	}
	
	public String getMonthName() {//get the month name
		return monthNames[monthNumber - 1];
	}
	
	public String toString() {//same as above
		return monthNames[monthNumber - 1];
	}
	
	public boolean equals(Month comparator) {//compare to months to see if they are the same
		if(comparator.monthNumber == monthNumber)
			return true;
		else
			return false;
	}
	
	public boolean greaterThan(Month comparator){//check if this month came after the comparison month
		if(comparator.monthNumber > monthNumber)
			return false;
		else
			return true;
	}
	
	public boolean lessThan(Month comparator){//check if this month came before the comparison month
		if(comparator.monthNumber > monthNumber)
			return true;
		else
			return false;
	}
	
}
