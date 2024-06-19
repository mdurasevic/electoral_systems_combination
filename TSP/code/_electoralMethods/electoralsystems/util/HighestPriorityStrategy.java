package _electoralMethods.electoralsystems.util;

public class HighestPriorityStrategy implements WinnerStrategy {
	@Override
	public int winner(int minPriorityIndex, int maxPriorityIndex) {
		return maxPriorityIndex;
	}

	@Override
	public int looser(int minPriorityIndex, int maxPriorityIndex) {
		return minPriorityIndex;
	}

	@Override
	public boolean leftIsBetter(double a, double b) {
		return a > b;
	}

	@Override
	public WinnerStrategy getInverse() {
		return new LowestPriorityStrategy();
	}
}