package chapter6;

public class MonthTest {

	public static void main(String[] args) {
		
		Month thisMonth = new Month();
		Month nextMonth = new Month(2);
		Month monthAfter = new Month("march");
		Month failMonth = new Month("derp");
		
		System.out.println(thisMonth.greaterThan(nextMonth));
		thisMonth.setMonthNumber(5);
		System.out.println(thisMonth.greaterThan(nextMonth));
		System.out.println(thisMonth.getMonthNumber());
		System.out.println(monthAfter.equals(nextMonth));
		monthAfter.setMonthNumber(5);
		System.out.println(monthAfter.equals(thisMonth));
		System.out.println(nextMonth.getMonthName());
		System.out.println(monthAfter.lessThan(nextMonth));
		System.out.println(failMonth.getMonthName());
	}

}
