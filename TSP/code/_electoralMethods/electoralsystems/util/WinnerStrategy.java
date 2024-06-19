package _electoralMethods.electoralsystems.util;

public interface WinnerStrategy {
	public int winner(int minPriorityIndex, int maxPriorityIndex);

	public int looser(int minPriorityIndex, int maxPriorityIndex);

	public boolean leftIsBetter(double a, double b);

	public WinnerStrategy getInverse();
}