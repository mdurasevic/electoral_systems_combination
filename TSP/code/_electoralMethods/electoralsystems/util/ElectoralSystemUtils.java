package _electoralMethods.electoralsystems.util;

import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class ElectoralSystemUtils {

	public static int getNumberOfDecisions(List<List<Double>> priorities) {
		return priorities.get(0).size();
	}

	public static int getNumberOfVoters(List<List<Double>> priorities) {
		return priorities.size();
	}

	public static List<Integer> createInitialCandidateList(int numberOfCandidates) {
		List<Integer> remainingCandidates = new ArrayList<Integer>();
		for (int i = 0; i < numberOfCandidates; i++) {
			remainingCandidates.add(i);
		}
		return remainingCandidates;
	}

	public static List<Integer> getIndicesWithLowestVoteCount(List<Double> votes, List<Integer> candidateIndices) {
		double minVotes = votes.get(candidateIndices.get(0));
		for (int i = 1; i < candidateIndices.size(); i++) {
			double candidateVotes = votes.get(candidateIndices.get(i));
			if (minVotes > candidateVotes) {
				minVotes = candidateVotes;
			}
		}
		List<Integer> winnerIndices = new ArrayList<Integer>();
		for (int candidateIndex : candidateIndices) {
			if (votes.get(candidateIndex).equals(minVotes)) {
				winnerIndices.add(candidateIndex);
			}
		}
		return winnerIndices;
	}

	public static List<Integer> getIndicesWithHighestVoteCount(List<Double> votes, List<Integer> candidateIndices) {
		double maxVotes = votes.get(candidateIndices.get(0));
		for (int i = 1; i < candidateIndices.size(); i++) {
			double candidateVotes = votes.get(candidateIndices.get(i));
			if (maxVotes < candidateVotes) {
				maxVotes = candidateVotes;
			}
		}
		List<Integer> winnerIndices = new ArrayList<Integer>();
		for (int candidateIndice : candidateIndices) {
			if (votes.get(candidateIndice) == maxVotes) {
				winnerIndices.add(candidateIndice);
			}
		}
		return winnerIndices;
	}

	public static List<Double> calculateRanksUnique(List<Double> priorities, WinnerStrategy winnerStrategy) {
		int n = priorities.size();
		List<Double> R = new ArrayList<>(Collections.nCopies(n, 0.0));
		List<Pair> T = new ArrayList<>(n);

		for (int i = 0; i < n; i++) {
			T.add(new Pair(priorities.get(i), i));
		}

		Collections.sort(T, new Comparator<Pair>() {
			@Override
			public int compare(Pair a, Pair b) {
				if (winnerStrategy.leftIsBetter(a.getFirst(), b.getFirst()))
					return -1;
				else if (a.getFirst() == b.getFirst())
					return 0;
				else
					return 1;
			}
		});

		double rank = 1;
		int i = 0;

		while (i < n) {
			int idx = T.get(i).getSecond();
			R.set(idx, rank);
			rank += 1;
			i += 1;
		}

		return R;
	}

	public static int determineVoteFromSubset(double[] voter, WinnerStrategy winnerStrategy, List<Integer> candidates) {
		final int initialCandidate = candidates.get(0);
		int min_index = initialCandidate;
		int max_index = initialCandidate;

		final double initialCandidatePriority = voter[initialCandidate];
		double minValue = initialCandidatePriority;
		double maxValue = initialCandidatePriority;

		for (int candidate : candidates) {
			double priority = voter[candidate];
			if (priority > maxValue) {
				maxValue = priority;
				max_index = candidate;
			}

			if (priority < minValue) {
				minValue = priority;
				min_index = candidate;
			}
		}

		final int winnerIndex = winnerStrategy.winner(min_index, max_index);
		return winnerIndex;
	}

	public static int determineVoteFromSubset(List<Double> voter, WinnerStrategy winnerStrategy,
			List<Integer> candidates) {
		int initialCandidate = candidates.get(0);
		int minIndex = initialCandidate;
		int maxIndex = initialCandidate;
		double initialCandidatePriority = voter.get(initialCandidate);
		double minValue = initialCandidatePriority;
		double maxValue = initialCandidatePriority;
		for (int candidate : candidates) {
			double priority = voter.get(candidate);
			if (priority > maxValue) {
				maxValue = priority;
				maxIndex = candidate;
			}

			if (priority < minValue) {
				minValue = priority;
				minIndex = candidate;
			}
		}
		return winnerStrategy.winner(minIndex, maxIndex);
	}

	public static List<Integer> determineApprovedCandidates(List<Double> priorities, WinnerStrategy winnerStrategy,
			double approvalRate) {
		int n = priorities.size();
		List<Integer> approvedCandidates = new ArrayList<>(Collections.nCopies(n, 0));
		List<Pair> T = new ArrayList<>(n);

		for (int i = 0; i < n; i++) {
			T.add(new Pair(priorities.get(i), i));
		}

		Collections.sort(T, new Comparator<Pair>() {
			@Override
			public int compare(Pair a, Pair b) {
				if (winnerStrategy.leftIsBetter(a.getFirst(), b.getFirst()))
					return -1;
				else if (a.getFirst() == b.getFirst())
					return 0;
				else
					return 1;
				}
		});

		
		
		int numberOfApproved = (int) (approvalRate * priorities.size());
		numberOfApproved = (numberOfApproved == 0) ? 1 : numberOfApproved;

		for (int i = 0; i < numberOfApproved; i++) {
			approvedCandidates.set(T.get(i).getSecond(), 1);
		}

		return approvedCandidates;
	}

	public static List<Integer> determineApprovedCandidatesByInterval(List<Double> priorities,
			WinnerStrategy winnerStrategy, double approvalRate) {
		List<Integer> approvedCandidates = new ArrayList<>(priorities.size());

		double min = Double.MAX_VALUE;
		double max = -Double.MAX_VALUE;

		for (double priority : priorities) {
			min = Math.min(min, priority);
			max = Math.max(max, priority);
		}

		double bestPriority = winnerStrategy.winner(0, 1);
		double interval = (max - min) * approvalRate;

		if (bestPriority == 0) {
			double upperBound = min + interval;
			for (int i = 0; i < priorities.size(); i++) {
				if (priorities.get(i) <= upperBound) {
					approvedCandidates.add(1);
				} else {
					approvedCandidates.add(0);
				}
			}
		} else {
			double lowerBound = max - interval;
			for (int i = 0; i < priorities.size(); i++) {
				if (priorities.get(i) >= lowerBound) {
					approvedCandidates.add(1);
				} else {
					approvedCandidates.add(0);
				}
			}
		}

		return approvedCandidates;

	}

	public static int determineDecisionWithRank(List<Double> ranking, int rank) {
		for (int i = 0; i < ranking.size(); i++) {
			if (ranking.get(i) == rank) {
				return i;
			}
		}
		return 0;
	}

	public static List<List<Integer>> calculateCondorcetMatrix(List<List<Double>> ranks) {
		int numberOfCandidates = ranks.get(0).size();
		List<List<Integer>> condorcetMatrix = new ArrayList<List<Integer>>();

		for (int i = 0; i < numberOfCandidates; i++) {
			List<Integer> row = new ArrayList<Integer>();
			for (int j = 0; j < numberOfCandidates; j++) {
				row.add(0);
			}
			condorcetMatrix.add(row);
		}
		for (List<Double> voter : ranks) {
			for (int i = 0; i < voter.size(); i++) {
				for (int j = i + 1; j < voter.size(); j++) {
					if (voter.get(i) < voter.get(j)) {
						condorcetMatrix.get(i).set(j, condorcetMatrix.get(i).get(j) + 1);
					} else {
						condorcetMatrix.get(j).set(i, condorcetMatrix.get(j).get(i) + 1);
					}
				}
			}
		}
		return condorcetMatrix;
	}

	public static int determineCondorcetWinner(List<List<Integer>> sumMatrix) {
		for (int i = 0; i < sumMatrix.size(); i++) {
			boolean isCondorcetWinner = true;
			for (int j = 0; j < sumMatrix.size(); j++) {
				if (i != j && sumMatrix.get(i).get(j) < sumMatrix.get(j).get(i)) {
					isCondorcetWinner = false;
				}
			}
			if (isCondorcetWinner) {
				return i;
			}
		}
		return -1;
	}

	public static int determineVote(List<Double> voter, WinnerStrategy winnerStrategy) {
		int minIndex = 0;
		int maxIndex = 0;
		double minValue = voter.get(0);
		double maxValue = voter.get(0);

		for (int i = 1; i < voter.size(); i++) {
			double priority = voter.get(i);
			if (priority < minValue) {
				minValue = priority;
				minIndex = i;
			}
			if (priority > maxValue) {
				maxValue = priority;
				maxIndex = i;
			}
		}

		int winnerIndex = winnerStrategy.winner(minIndex, maxIndex);
		return winnerIndex;
	}

	public static boolean hasMajority(List<Double> votes, int candidate, int voterCount) {
		return (voterCount / 2) < votes.get(candidate);
	}

	public static List<Integer> determineSecondBestCandidates(List<Double> votes, int winnerIndex) {
		List<Integer> secondBestIndices = new ArrayList<>();
		double secondBestVoteCount = 0;

		for (int i = 0; i < votes.size(); i++) {
			double voteCount = votes.get(i);
			if (i != winnerIndex && voteCount > secondBestVoteCount) {
				secondBestVoteCount = voteCount;
				secondBestIndices.clear();
				secondBestIndices.add(i);
			} else if (voteCount == secondBestVoteCount) {
				secondBestIndices.add(i);
			}
		}

		return secondBestIndices;
	}

	public static List<List<Double>> calculateSmithPreferenceMatrix(List<List<Double>> ranks) {
		List<List<Integer>> concordateMatrix = new ArrayList<>();
		int numberOfCandidates = ranks.get(0).size();

		for (int i = 0; i < numberOfCandidates; i++) {
			concordateMatrix.add(new ArrayList<Integer>(Collections.nCopies(numberOfCandidates, 0)));
		}

		for (List<Double> voter : ranks) {
			for (int i = 0; i < voter.size(); i++) {
				for (int j = i + 1; j < voter.size(); j++) {
					if (voter.get(i) < voter.get(j)) {
						concordateMatrix.get(i).set(j, concordateMatrix.get(i).get(j) + 1);
					} else {
						concordateMatrix.get(j).set(i, concordateMatrix.get(j).get(i) + 1);
					}
				}
			}
		}

		List<List<Double>> preferenceMatrix = new ArrayList<>();
		for (int i = 0; i < concordateMatrix.size(); i++) {
			preferenceMatrix.add(new ArrayList<Double>(Collections.nCopies(numberOfCandidates, 0.0)));
		}

		for (int i = 0; i < concordateMatrix.size(); i++) {
			for (int j = i + 1; j < concordateMatrix.size(); j++) {
				if (concordateMatrix.get(i).get(j) > concordateMatrix.get(j).get(i)) {
					preferenceMatrix.get(i).set(j, 1.0);
				} else if (concordateMatrix.get(i).get(j) == concordateMatrix.get(j).get(i)) {
					preferenceMatrix.get(i).set(j, 0.5);
					preferenceMatrix.get(j).set(i, 0.5);
				} else {
					preferenceMatrix.get(j).set(i, 1.0);
				}
			}
		}
		return preferenceMatrix;

	}

	public static List<Double> calculateCopelandScoreFromPreferenceMatrix(List<List<Double>> preferences,
			List<Integer> candidates) {
		int numberOfCandidates = preferences.get(0).size();
		List<Double> wins = new ArrayList<Double>(numberOfCandidates);

		for (int i = 0; i < numberOfCandidates; i++) {
			wins.add(0.0);
		}

		for (int i = 0; i < candidates.size(); i++) {
			for (int j = 0; j < candidates.size(); j++) {
				wins.set(i, wins.get(i) + preferences.get(candidates.get(i)).get(candidates.get(j)));
			}
		}
		return wins;
	}

	public static List<Double> normaliseScores(List<Double> voter, WinnerStrategy winnerStrategy, int maximumScore,
			int minimumScore, List<Integer> candidateIndices) {
		double initialCandidatePriority = voter.get(candidateIndices.get(0));

		double maxVotes = initialCandidatePriority;
		double minVotes = initialCandidatePriority;
		double totalSum = initialCandidatePriority;
		for (int i = 1; i < candidateIndices.size(); i++) {
			double candidatePriority = voter.get(candidateIndices.get(i));
			if (maxVotes < candidatePriority) {
				maxVotes = candidatePriority;
			}
			if (minVotes > candidatePriority) {
				minVotes = candidatePriority;
			}
			totalSum += Math.abs(candidatePriority);
		}

		List<Double> normalisedScores = new ArrayList<Double>(Collections.nCopies(voter.size(), 0.0));

		if (winnerStrategy.leftIsBetter(0, 1)) {
			totalSum -= maxVotes * candidateIndices.size();
			totalSum *= -1;
		} else {
			totalSum -= minVotes * candidateIndices.size();
		}

		double ratio = (maximumScore - minimumScore) / totalSum;

		for (int candidateIndice : candidateIndices) {
			if (winnerStrategy.leftIsBetter(0, 1)) {
				normalisedScores.set(candidateIndice, (maxVotes - voter.get(candidateIndice)) * ratio);
			} else {
				normalisedScores.set(candidateIndice, (voter.get(candidateIndice) - minVotes) * ratio);
			}
		}

		return normalisedScores;
	}

	public static List<Double> calculateScoresUniform(List<Double> voter, WinnerStrategy winnerStrategy,
			int maximumScore, int minimumScore) {
		List<Double> scores = new ArrayList<Double>();
		double maxPriority = -Double.MAX_VALUE;
		double minPriority = Double.MAX_VALUE;

		for (double priority : voter) {
			if (priority > maxPriority) {
				maxPriority = priority;
			}
			if (priority < minPriority) {
				minPriority = priority;
			}
		}

		double worstValue;
		int better = winnerStrategy.winner(0, 1);
		if (better == 0) {
			worstValue = maxPriority;
		} else {
			worstValue = minPriority;
		}

		for (double priority : voter) {
			double denominator = (maxPriority - minPriority);
			double score;
			if (Math.abs(denominator) < 0.0000001) {
				score = maximumScore;
			} else {
				score = (int) Math
						.round(minimumScore + (maximumScore - minimumScore) * (priority - worstValue) / denominator);
			}

			if (score < 0) {
				score *= -1;
			}
			scores.add(score);
		}

		return scores;

	}

	public static double calculateMedian(List<Double> scores) {
		int n = scores.size() / 2;
		Collections.sort(scores);
		return scores.get(n);
	}

	public static int calculatePermutationScore(List<Integer> order, List<List<Integer>> preferences) {
		int score = 0;
		for (int i = 0; i < order.size(); i++) {
			for (int j = i + 1; j < order.size(); j++) {
				score += preferences.get(order.get(i)).get(order.get(j));
			}
		}
		return score;
	}

	public static List<Integer> getIndicesBelowAverage(int[] scores, List<Integer> candidateIndices,
			double averageScore) {
		List<Integer> loserIndices = new ArrayList<>();
		for (int candidateIndex : candidateIndices) {
			if (scores[candidateIndex] < averageScore) {
				loserIndices.add(candidateIndex);
			}
		}
		return loserIndices;
	}

	public static List<List<Integer>> calculateTallyMatrix(List<List<Double>> ranks) {
		int numberOfCandidates = ranks.get(0).size();
		List<List<Integer>> condorcetMatrix = new ArrayList<>();
		for (int i = 0; i < numberOfCandidates; i++) {
			condorcetMatrix.add(new ArrayList<>());
			for (int j = 0; j < numberOfCandidates; j++) {
				condorcetMatrix.get(i).add(0);
			}
		}
		for (List<Double> voter : ranks) {
			for (int i = 0; i < voter.size(); i++) {
				for (int j = i + 1; j < voter.size(); j++) {
					if (voter.get(i) < voter.get(j)) {
						condorcetMatrix.get(i).set(j, condorcetMatrix.get(i).get(j) + 1);
						condorcetMatrix.get(j).set(i, condorcetMatrix.get(j).get(i) - 1);
					} else {
						condorcetMatrix.get(i).set(j, condorcetMatrix.get(i).get(j) - 1);
						condorcetMatrix.get(j).set(i, condorcetMatrix.get(j).get(i) + 1);
					}
				}
			}
		}
		return condorcetMatrix;
	}

	public static List<List<Integer>> calculateStrongestPaths(List<List<Integer>> sumMatrix) {
		final int numberOfCandidates = sumMatrix.get(0).size();
		List<List<Integer>> strengthMatrix = new ArrayList<>();

		for (int i = 0; i < numberOfCandidates; i++) {
			strengthMatrix.add(new ArrayList<>());
			for (int j = 0; j < numberOfCandidates; j++) {
				strengthMatrix.get(i).add(0);
			}
		}

		for (int i = 0; i < numberOfCandidates; i++) {
			for (int j = 0; j < numberOfCandidates; j++) {
				if (i != j) {
					if (sumMatrix.get(i).get(j) > sumMatrix.get(j).get(i)) {
						strengthMatrix.get(i).set(j, sumMatrix.get(i).get(j));
					}
				}
			}
		}

		for (int i = 0; i < numberOfCandidates; i++) {
			for (int j = 0; j < numberOfCandidates; j++) {
				if (i != j) {
					for (int k = 0; k < numberOfCandidates; k++) {
						if (i != k && j != k) {
							strengthMatrix.get(j).set(k, Math.max(strengthMatrix.get(j).get(k),
									Math.min(strengthMatrix.get(j).get(i), strengthMatrix.get(i).get(k))));
						}
					}
				}
			}
		}
		return strengthMatrix;
	}

	public static List<Double> calculatePairwiseDefeatMargins(List<List<Integer>> sumMatrix) {
		List<Double> defeats = new ArrayList<>(sumMatrix.size());
		for (int i = 0; i < sumMatrix.size(); i++) {
			defeats.add(-Double.MAX_VALUE);
		}
		for (int i = 0; i < sumMatrix.size(); i++) {
			for (int j = 0; j < sumMatrix.size(); j++) {
				if (i == j) {
					continue;
				}
				double defeat = sumMatrix.get(j).get(i) - sumMatrix.get(i).get(j);
				if (defeat > defeats.get(i)) {
					defeats.set(i, defeat);
				}
			}
		}
		return defeats;
	}

	public static List<List<Double>> calculatePreferenceMatrix(List<List<Double>> ranks) {
		List<List<Double>> concordateMatrix = new ArrayList<>();
		int numberOfCandidates = ranks.get(0).size();
		for (int i = 0; i < numberOfCandidates; i++) {
			concordateMatrix.add(new ArrayList<>(numberOfCandidates));
			for (int j = 0; j < numberOfCandidates; j++) {
				concordateMatrix.get(i).add(0.0);
			}
		}
		for (List<Double> voter : ranks) {
			for (int i = 0; i < voter.size(); i++) {
				for (int j = i + 1; j < voter.size(); j++) {
					if (voter.get(i) > voter.get(j)) {
						concordateMatrix.get(i).set(j, concordateMatrix.get(i).get(j) + 1);
					} else {
						concordateMatrix.get(j).set(i, concordateMatrix.get(j).get(i) + 1);
					}
				}
			}
		}

		return concordateMatrix;
	}

	public static List<Integer> sortByScores(List<Integer> indices, List<Double> scores,
			WinnerStrategy winnerStrategy) {
		int n = indices.size();
		List<Integer> sortedIndices = new ArrayList<>();

		List<Pair> T = new ArrayList<>(n);

		for (int i = 0; i < n; i++) {
			T.add(new Pair(scores.get(i), indices.get(i)));
		}

		Collections.sort(T, new Comparator<Pair>() {
			@Override
			public int compare(Pair a, Pair b) {
				if (winnerStrategy.leftIsBetter(a.getFirst(), b.getFirst()))
					return -1;
				else if (a.getFirst() == b.getFirst())
					return 0;
				else
					return 1;			}
		});

		for (Pair t : T) {
			sortedIndices.add(t.getSecond());
		}

		return sortedIndices;
	}
}