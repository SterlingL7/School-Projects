package chapter10;
/*
 * Sterling LaBarbera
 * Chapter 10 Programming Challenge 1
 * Test Scores
 * 
 * This class holds an array of scores and can return the average
 * if a value is negative or greater than 100 it throws an error
 */
public class TestScores {
	double[] testScores;
	
	public TestScores(double[] scores) {
		testScores = new double[scores.length];//make the local array the same size as the argument array
		
		for(int x = 0; x < testScores.length; x++) {//move each score into the class's array with a loop
			if(scores[x] > 100 || scores[x] < 0)//test for valid score
				throw new IllegalArgumentException("Element: " + x + " Score: " + scores[x]);
			testScores[x] = scores[x];
		}
	}
	
	public double getAverage() {//find the average of the scores
		double total = 0;
		
		for(int x = 0; x < testScores.length; x++) {
			total+=testScores[x];//accumulate the scores
		}
		
		total = total/testScores.length;//divide by # of scores
		
		return total;
	}
}
