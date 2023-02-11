package Chapter8;

/*Sterling LaBarbera
 * Chapter 7
 * Programming Challenge 2
 * 
 * this is the main program for updating and showing employee data
 * gets all the payroll data for each employee in a loop
 * then prints out the gross pay for each employee
 */
import java.util.Scanner;

public class BackwardString {

	public static void main(String[] args) {
		Scanner keyboard = new Scanner(System.in);
		String testString, resultString;
		
		System.out.println("Input a test String: ");// get string input from user
		testString = keyboard.nextLine();
		resultString = reverseString(testString);//call the method
		
		System.out.println(resultString);//print the reversed string
	}

	public static String reverseString(String target) {//this method takes a string, converts to char array, and swaps the order of each character
		char[] swapping = target.toCharArray();//convert the string to an array
		char temp;						//used as a buffer for element swapping
		int y = swapping.length - 1;//index for the end of the array
		int x = 0;
		
		while(x < y){// swap each element in the array, first with last, 2nd with 2nd to last, etc
			temp = swapping[x];
			swapping[x] = swapping[y];
			swapping[y] = temp;
			x++;
			y--;
		}
		
		String retrn = new String(swapping);//convert the char array to a string
		return retrn;
	}
}
