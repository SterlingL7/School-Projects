package chapter9;

public class CourseGradesDemo {
	public static void main(String[] args) {
		// Create an object for the lab grade.
		GradedActivity lab = new GradedActivity();
		// Set the lab score at 85.
		lab.setScore(85);

		// Create an object for the pass/fail exam.
		// 20 total questions, 3 questions missed, minimum
		// passing score is 70.
		PassFailExam pfexam = new PassFailExam(20, 4, 70);

		// Create an object for the essay grade.
		Essay essay = new Essay();
		// Set the essay scores. Grammar = 25, spelling = 18
		// length = 17, content = 20.
		essay.setScore(10, 10, 10, 10);

		// Create an object for the final exam.
		// 50 questions, 10 missed.
		FinalExam finalExam = new FinalExam(50, 20);

		// Create a CourseGrades object.
		CourseGrades course = new CourseGrades();
		course.setLab(lab);
		course.setPassFailExam(pfexam);
		course.setEssay(essay);
		course.setFinalExam(finalExam);

		// Display the scores and grades.
		System.out.println(course);
	}
}
