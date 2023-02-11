package chapter4;

public class SoftwareSales {
	
	private int units;
	private float price = 99;
	private float currentDiscount;
		
	public SoftwareSales(int noSold){
		units = noSold;
		
		if(units<10)
			currentDiscount = 0;
		else if(units>=10 && units<20)
			currentDiscount = (float) (price*units*.2);
		else if(units>=20 && units<50)
			currentDiscount = (float) (price*units*.3);
		else if(units>=50 && units<100)
			currentDiscount = (float) (price*units*.4);
		else
			currentDiscount = (float) (price*units*.5);
	}
	
	public int getUnitsSold(){
		return units;
	}
	
	public float getDiscount(){
		return currentDiscount;
	}
	
	public float getCost(){
		return price*units-currentDiscount;
	}
}
