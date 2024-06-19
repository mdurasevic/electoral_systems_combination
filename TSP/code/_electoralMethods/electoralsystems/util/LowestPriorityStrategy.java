package _electoralMethods.electoralsystems.util;

public class LowestPriorityStrategy implements WinnerStrategy {
    public int winner(int minPriorityIndex, int maxPriorityIndex) {
        return minPriorityIndex;
    }

    public int looser(int minPriorityIndex, int maxPriorityIndex) {
        return maxPriorityIndex;
    }

    public boolean leftIsBetter(double a, double b) {
        return a < b;
    }

    public WinnerStrategy getInverse() {
        return new HighestPriorityStrategy();
    }
}