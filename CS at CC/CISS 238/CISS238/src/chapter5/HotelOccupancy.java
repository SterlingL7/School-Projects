package chapter5;

import java.util.Scanner;

public class HotelOccupancy {


	public static void main(String[] args) {
		Scanner keyboard = new Scanner(System.in);//set up input scanner
		
		int floors, rooms, fullrooms;//basic values
		int occupiedRooms = 0;
		int totalRooms = 0;//accumulators 
		double occupancyRate = 0;//the thing we want, will be set to occupied rooms / total rooms
		
		System.out.println("How many floors does the hotel have? ");//get number of floors
		floors = keyboard.nextInt();
		while(floors<1) {//hotels cannot have 0 or fewer floors
			System.out.println("Invalid. Enter 1 or more: ");
			floors = keyboard.nextInt();
		}
			
		for(int x=1; x<=floors; x++) {//find out the number of rooms on each floor starting from the top
			System.out.println("How many rooms does floor " + x + " have? ");
			rooms = keyboard.nextInt();
			
			while(rooms<10) {//force user to enter a valid number of rooms
				System.out.println("Invalid. Enter 10 or more: ");
				rooms = keyboard.nextInt();
			}
			
			totalRooms+=rooms; //increment the room count by number of rooms on this floor
			
			System.out.println("How many occupied rooms does floor " + x + " have? ");
			fullrooms = keyboard.nextInt();//increment the occupied room count for this floor
			
			while(fullrooms > rooms) {//make sure the number of occupied rooms is not larger than the number of rooms on a floor
				System.out.println("Invalid. Enter " + rooms + " or fewer: ");
				fullrooms = keyboard.nextInt();
			}
			occupiedRooms += fullrooms;
			
		}
		occupancyRate = occupiedRooms/(double)totalRooms*100;//set the occupancy rate
		System.out.println(occupancyRate);
		System.out.println("Number of rooms: " + totalRooms + "\n"
				+ "Occupied Rooms: " + occupiedRooms + "\n"
				+ "Vacant rooms: " + (totalRooms-occupiedRooms) + "\n"
				+ "Occupancy rate: " + occupancyRate + " %");
	}
}