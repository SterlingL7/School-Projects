package chapter9;
/*
 * Sterling LaBarbera
 * Chapter 9
 * Programming Challenge 5
 * 
 * this class compiles the individual grade classes into a single array of grade objects
 * the methods for filling in the array objects generally extract the data from the external objects.
 */
public class CourseGrades {
		
	private GradedActivity[] grades = new GradedActivity[4];//array of the superclass for holding each grade
	
	public void setLab(GradedActivity labs) {//set the score for the lab
		grades[0] = new GradedActivity();
		grades[0].setScore(labs.getScore());
	}
	
	public void setPassFailExam(PassFailExam pfexam) {//set the P/F exam score
		grades[1] = new PassFailExam(20, pfexam.getNumMissed(), 70);//cannot get the # of questions or passing score from other objects
	}
	
	public void setEssay(Essay essay) {//set the essay scores
		grades[2] = new Essay();
		((Essay)grades[2]).setScore(essay.getGrammar(), essay.getSpelling(), essay.getCorrectLength(), essay.getContent());
		//cast as Essay since to use correct setScore() method
	}
	
	public void setFinalExam(FinalExam finalExam) {//set the final exam scores
		grades[3] = new FinalExam(50, finalExam.getNumMissed());
	}
	
	public String toString() {//convert the class data to a formatted string
		String gradeString = "Lab Score: " + grades[0].getScore() + "	Grade: " + grades[0].getGrade() +
				"\nPass/Fail Exam Score: " + grades[1].getScore() + "	Grade: " + grades[1].getGrade() +
				"\nEssay Score: " + grades[2].getScore() + "	Grade: " + grades[2].getGrade() +
				"\nFinal Exam Score: " + grades[3].getScore() + "	Grade: " + grades[3].getGrade();
		return gradeString;
	}
}
