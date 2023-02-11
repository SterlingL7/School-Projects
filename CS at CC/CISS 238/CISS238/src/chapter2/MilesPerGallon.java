package chapter2;
/*Sterling LaBarbera
  Chapter 2
  Programming Challenge 9
  
  This program takes 2 float values, one for miles, and one for gallons
  and calculates miles per gallon
  Can be used by multiple people in a row
*/


import java.util.Scanner;
import javax.swing.JOptionPane;

public class MilesPerGallon {

	public static void main(String[] args) {
		
		Scanner keyboard = new Scanner(System.in);//set up input scanner
		
		String name;
		String tempstring;
		float miles;
		float gallons;
		float MPG;
		String yesno = "n";//continue variable

		do {
			tempstring = "";
			name = JOptionPane.showInputDialog("Enter your name: ");//get name
			
			tempstring = JOptionPane.showInputDialog("Please enter the number of miles you travelled before your last fill up.");//get miles
			miles = Float.parseFloat(tempstring);
			
			tempstring = JOptionPane.showInputDialog("\nPlease input the number of gallons you purchased at last fill up.");//get gallons
			gallons = Float.parseFloat(tempstring);

			MPG = miles/gallons;//calculate the miles per gallon
			
			JOptionPane.showMessageDialog(null, name + ", your miles per gallon are: " + MPG);//give user the value
			yesno = JOptionPane.showInputDialog("Would you like to calculate again? y/n");//see if program should continue
									
		}while(yesno.charAt(0) == 'y' || yesno.charAt(0) == 'Y');//not case sensitive
		System.exit(0);
	}
}
