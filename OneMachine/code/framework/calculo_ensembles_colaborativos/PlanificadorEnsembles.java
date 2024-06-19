package framework.calculo_ensembles_colaborativos;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import _experimentosElectoralMethods.SEC_adicionalSCORE;
import dominio.Intervalo;
import dominio.OperacionesAsignacionTrabajosMaquina;
import dominio.Planificador;
import framework.calculo_ensembles_colaborativos.electoralsystems.util.ElectoralSystemUtils;
import framework.calculo_ensembles_colaborativos.electoralsystems.util.HighestPriorityStrategy;
import framework.calculo_ensembles_colaborativos.electoralsystems.util.LowestPriorityStrategy;
import framework.calculo_ensembles_colaborativos.electoralsystems.util.Triple;
import framework.calculo_ensembles_colaborativos.electoralsystems.util.WinnerStrategy;
import framework.core.AlgoUtil;

public class PlanificadorEnsembles extends Planificador {

	public Ensemble_Colaborativo ensemble;
	public Intervalo intervalo;
	protected WinnerStrategy winnerStrategy = new HighestPriorityStrategy();

	public PlanificadorEnsembles(Ensemble_Colaborativo ensemble) {
		this.ensemble = ensemble;
	}

	@Override
	protected int buscarMejorTarea(Intervalo intervalo) {
		if (sin_asignar.size() == 1)
			return sin_asignar.get(0);
		this.intervalo = intervalo;
		if (AlgoUtil.gestorReglas.combinationMethod == -1)
			return suma(intervalo);
		List<List<Double>> priorities = priorities(intervalo);
		if (feasibleOptions.isEmpty())
			return 0;
		return feasibleOptions.get(electoralMethod(priorities));
	}

	public List<Integer> feasibleOptions = new ArrayList<Integer>();

	private List<List<Double>> priorities(Intervalo intervalo) {
		int nReglas = ensemble.getSizeReal();
		List<List<Double>> prioridades = new ArrayList<>();
		feasibleOptions.clear();
		double priorityRule;
		for (int regla = 0; regla < nReglas; regla++) {
			List<Double> choices = new ArrayList<Double>();
			for (Integer tarea : sin_asignar)
				if (intervalo.libre() && OperacionesAsignacionTrabajosMaquina.propaga(
						OperacionesAsignacionTrabajosMaquina.creaIntervalo(intervalo), (int) instancia.p[tarea],
						OperacionesAsignacionTrabajosMaquina.copiaIntervalos(intervalos))) {
					priorityRule = ensemble.get(regla).calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo);
					if (!Double.isFinite(priorityRule))
						priorityRule = 0;
					choices.add(priorityRule);
					if (!feasibleOptions.contains(tarea))
						feasibleOptions.add(tarea);
				}
			prioridades.add(choices);
		}
		return prioridades;
	}

	/*
	public static String[] names = new String[] { "AntiPlurality", "ApprovalVoting", "Baldwin", "BordaStandard",
			"BordaNauru", "BordaBinaryNumberSystem", "BordaEurovision", "BucklinVoting", "CombinedApprovalVoting",
			"CombinedWithIntervalApprovalVoting", "Condorcet", "ContingenVoting", "Coombs", "Copeland",
			"DistributedVoting", "InstantRunoff", "JudgementCentral", "JudgementMajority", "JudgementTypical",
			"JudgementUsual", "KemenyYoung", "Majority", "MiniMax", "Nanson", "Oklahoma", "Plurality",
			"PluralitySecondRound", "PreferentialBlockVoting", "RankedPairs", "Schulze", "ScoreVoting", "StarVoting",
			"TidemanAlternativeSmith", "UnifiedPrimary", "Suma" };
*/
	public static String[] names = new String[] { "AntiPlurality", "ApprovalVoting", "Baldwin", "BordaStandard",
			"BordaNauru", "BordaBinaryNumberSystem", "BordaEurovision", "BucklinVoting", "CombinedApprovalVoting",
			"CombinedWithIntervalApprovalVoting", "Condorcet", "ContingenVoting", "Coombs", "Copeland",
			"DistributedVoting", "InstantRunoff", "JudgementCentral", "JudgementMajority", "JudgementTypical",
			"JudgementUsual", "KemenyYoung", "Majority", "MiniMax", "Nanson", "Oklahoma", "Plurality",
			"PluralitySecondRound", "PreferentialBlockVoting", "RankedPairs", "Schulze", "ScoreVoting", "StarVoting",
			"TidemanAlternativeSmith", "UnifiedPrimary", "UnifiedPrimaryInterval", "Suma" };
	
	public static String getPlanificador(int i) {
		return names[i];
	}

	private int electoralMethod(List<List<Double>> priorities) {
		switch (AlgoUtil.gestorReglas.combinationMethod) {
		case 0:
			return antiPlurality(priorities);
		case 1:
			approvalRate = 0.3;
			useApprovalRange = false;
			useSecondRound = true;
			return approvalVoting(priorities);
		case 2:
			return baldwinMethod(priorities);
		case 3:
			return bordaCount(priorities, 0);
		case 4:
			return bordaCount(priorities, 1);
		case 5:
			return bordaCount(priorities, 2);
		case 6:
			return bordaCount(priorities, 3);
		case 7:
			return bucklinVoting(priorities);
		case 8:
			approvalRate = 0.3;
			disapprovalRate = 0.3;
			useApprovalInterval = false;
			return combinedApprovalVoting(priorities);
		case 9:
			approvalRate = 0.3;
			disapprovalRate = 0.3;
			useApprovalInterval = true;
			return combinedApprovalVoting(priorities);	
		case 10:
			return condorcetMethod(priorities);
		case 11:
			return contingenVoting(priorities);
		case 12:
			return coombsMethod(priorities);
		case 13:
			return copelandMethod(priorities);
		case 14:
			return distributedVoting(priorities);
		case 15:
			return instantRunoff(priorities);
		case 16:
			minimumRating = 1;
			maximumRating = 5;
			medianRule = MedianRule.CentralJudgement;
			return judgementMethod(priorities);
		case 17:
			minimumRating = 1;
			maximumRating = 5;
			medianRule = MedianRule.MajorityJudgement;
			return judgementMethod(priorities);
		case 18:
			minimumRating = 1;
			maximumRating = 5;
			medianRule = MedianRule.TypicalJudgement;
			return judgementMethod(priorities);
		case 19:
			minimumRating = 1;
			maximumRating = 5;
			medianRule = MedianRule.UsualJudgement;
			return judgementMethod(priorities);
		case 20:
			return kemenyYoungMethod(priorities);
		case 21:
			return majorityVote(priorities);
		case 22:
			return miniMax(priorities);
		case 23:
			return nansonMethod(priorities);
		case 24:
			return oklahomaMethod(priorities);
		case 25:
			return pluralityVote(priorities);
		case 26:
			return pluralityVote2round(priorities);
		case 27:
			return preferentialBlockVoting(priorities);
		case 28:
			return rankedPairs(priorities);
		case 29:
			return schulzeMethod(priorities);
		case 30:
			return scoreVoting(priorities);
		case 31:
			return starVoting(priorities);
		case 32:
			return tidemanAlternativeSmith(priorities);
		case 33:
			approvalRate = 0.3;
			useApprovalInterval = false;
			return unifiedPrimary(priorities);
		case 34:
			approvalRate = 0.3;
			useApprovalInterval = true;
			return unifiedPrimary(priorities);	
		case 35:
			return suma(priorities);
		}
		return -1;
	}

	/*
	 * Electoral methods
	 */
	private int antiPlurality(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		WinnerStrategy highest = new HighestPriorityStrategy();
		List<List<Double>> rankings = new ArrayList<>(voterCount);
//	        rankings.reserve(voterCount); ????

		// calculate the rankings of all candidates
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		do {
			List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

			// determine the votes for the candidates where the candidate was ranked the
			// lowest
			// largest rank means worst solution
			for (List<Double> voter : rankings) {
				int winner = ElectoralSystemUtils.determineVoteFromSubset(voter, highest, remainingCandidates);
				votes.set(winner, votes.get(winner) + 1);
			}

			// we then determine the winners as those candidates that received the lowest
			// number of votes against them
			List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithLowestVoteCount(votes,
					remainingCandidates);

			// If there is only one candidate that received the most votes against himself
			// he is the winner (no majority required)
			if (winnerIndices.size() == 1) {
				return winnerIndices.get(0);
			}

			// if the number of winners is the same as the number of remaining candidates,
			// we have a tie that we cannot resolve
			if (winnerIndices.size() == remainingCandidates.size()) {
				return winnerIndices.get(0);
			}

			remainingCandidates = new ArrayList<>(winnerIndices);

		} while (true);
	}

	double approvalRate;
	boolean useApprovalRange;
	boolean useSecondRound;

	private int approvalVoting(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		double currentApprovalRate = approvalRate;

		do {
			List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));
			boolean onlyOneCandidate = true;

			for (List<Double> voter : priorities) {
				List<Integer> approvals;

				if (!useApprovalRange) {
					// in this case there is a fixed percentage of candidates that can be approved
					approvals = ElectoralSystemUtils.determineApprovedCandidates(voter, winnerStrategy,
							currentApprovalRate);
				} else {
					// in this case the number of approved candidates can vary because the approval
					// rate is calculated as the range from the best priority
					approvals = ElectoralSystemUtils.determineApprovedCandidatesByInterval(voter, winnerStrategy,
							currentApprovalRate);
				}

				int approvalCount = 0;

				for (int i = 0; i < approvals.size(); i++) {
					approvalCount += approvals.get(i);
					votes.set(i, votes.get(i) + approvals.get(i));
				}

				// if each voter approved only a single candidate, we will switch to plurality
				// vote
				if (approvalCount > 1) {
					onlyOneCandidate = false;
				}
			}

			List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes,
					remainingCandidates);

			// if there is a single winner and we do not want to use a second round.
			if (winnerIndices.size() == 1 || !useSecondRound) {
				return winnerIndices.get(0);
			}

			// we reduce the approval rate by two and reduce the number of candidates that
			// can be approved by the voters
			currentApprovalRate /= 2;

			// switch to plurality vote in case we vote for only one candidate, or the
			// approval rate becomes too small.
			if (onlyOneCandidate || currentApprovalRate < 1e-6) {
				return pluralityVote(priorities);
			}
		} while (true);
	}

	private int baldwinMethod(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		// if there is a single candidate, we can return him as winner
		if (decisionCount == 1) {
			return 0;
		}

		// calculate the ranks for each candidate
		rankings = new ArrayList<>(voterCount);
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		do {
			List<Double> total = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

			// determine the scores for all candidates
			for (List<Double> ranking : rankings) {
				for (int candidateIndex : remainingCandidates) {
					total.set(candidateIndex, total.get(candidateIndex) + decisionCount - ranking.get(candidateIndex));
				}
			}

			// determine the candidates that received the lowest score based on the rankings
			List<Integer> looserIndices = ElectoralSystemUtils.getIndicesWithLowestVoteCount(total,
					remainingCandidates);
			if (looserIndices.size() == remainingCandidates.size()) {
				return remainingCandidates.get(0);
			}

			// we need to update the ranks of all those candidates that we will eliminate
			for (int looser : looserIndices) {
				for (List<Double> ranking : rankings) {
					// determine the rank of the looser
					double looserRank = ranking.get(looser);
					for (int remainingCandidate : remainingCandidates) {
						if (remainingCandidate == looser) {
							ranking.set(remainingCandidate, 1.0 * decisionCount + 1);
						} else if (looserRank < ranking.get(remainingCandidate)) {
							ranking.set(remainingCandidate, ranking.get(remainingCandidate) - 1);
						}
					}
				}
			}

			for (int i = looserIndices.size() - 1; i >= 0; i--) {
				remainingCandidates.remove(looserIndices.get(i));
			}

		} while (remainingCandidates.size() > 1);

		return remainingCandidates.get(0);
	}

	private int bordaCount(List<List<Double>> priorities, int mode) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));
		List<List<Double>> rankings = new ArrayList<>();
//        rankings.reserve(voterCount); ????

		// First determine the rankings
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
			// Here we immediately determine the votes in case there is a tie after the
			// Borda count
			int winner = ElectoralSystemUtils.determineVote(voter, winnerStrategy);
			votes.set(winner, votes.get(winner) + 1);
		}

		List<Double> total = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

		// Sum all the rankings based on the grading strategy
		for (int i = 0; i < rankings.size(); i++) {
			for (int j = 0; j < rankings.get(i).size(); j++) {
				total.set(j, total.get(j) + getValueStrategy(i, j, decisionCount, rankings, mode));
			}
		}

		// Determine the winner
		List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(total, remainingCandidates);

		// If there is more than one winner, try with plurality
		if (winnerIndices.size() > 1) {
			List<Integer> newWinnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes, winnerIndices);
			return newWinnerIndices.get(0);
		}

		return winnerIndices.get(0);
	}

	double[] eurovisionScores = new double[] { 12, 10, 8, 7, 6, 5, 4, 3, 2, 1 };

	private double getValueStrategy(int i, int j, int decisionCount, List<List<Double>> rankings, int mode) {
		if (mode == 0)
			return decisionCount - (rankings.get(i).get(j)); // Standard
		else if (mode == 1)
			return 1.0 / (rankings.get(i).get(j)); // Nauru
		else if (mode == 2)
			return 1.0 / Math.pow(2, rankings.get(i).get(j)); // BinaryNumberSysten
		else if (mode == 3) { // eurovision
			if (rankings.get(i).get(j) <= eurovisionScores.length) {
				return eurovisionScores[(int) (rankings.get(i).get(j) - 1)];
			}
		}
		return 0;

	}

	private int bucklinVoting(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<List<Double>> rankings = new ArrayList<>();
		List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));
		rankings = new ArrayList<>(priorities.size());

		// determine the ranks of all candidates
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		// calculate the number of votes required for a majority
		int majority = rankings.size() / 2 + 1;

		List<Integer> winners = null;
		// this loop needs to start with 1 because it represents the ranks, which start
		// from 1
		// the idea is to start with rank one and sum all the votes. if we have a
		// majority, the procedure ends
		// if no majority is reached, we add votes for the second preferences, and it is
		// checked whether a candidate has a majority of votes
		// the majority is not updated! it always stays the same. Thus at one point, we
		// should have a majority
		for (int i = 1; i <= decisionCount; i++) {
			for (List<Double> ranking : rankings) {
				int winner = ElectoralSystemUtils.determineDecisionWithRank(ranking, i);
				votes.set(winner, votes.get(winner) + 1);
			}
			winners = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes, remainingCandidates);
			if (winners.size() == 1 && votes.get(winners.get(0)) >= majority) {
				return winners.get(0);
			}
		}

		return winners.get(0);
	}

	double disapprovalRate;
	boolean useApprovalInterval;

	private int combinedApprovalVoting(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		double currentApprovalRate = approvalRate;
		double currentDisapprovalRate = disapprovalRate;

		WinnerStrategy inverseStrategy = winnerStrategy.getInverse();

		do {
			List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));
			boolean onlyOneCandidate = true;

			for (List<Double> voter : priorities) {
				List<Integer> approvals;
				if (!useApprovalInterval) {
					// in this case, there is a fixed percentage of candidates that can be approved
					approvals = ElectoralSystemUtils.determineApprovedCandidates(voter, winnerStrategy,
							currentApprovalRate);
				} else {
					// in this case, the number of approved candidates can vary because the approval
					// rate is calculated as the range from the best priority
					approvals = ElectoralSystemUtils.determineApprovedCandidatesByInterval(voter, winnerStrategy,
							currentApprovalRate);
				}
				int approvalCount = 0;
				for (int i = 0; i < approvals.size(); i++) {
					approvalCount += approvals.get(i);
					votes.set(i, votes.get(i) + approvals.get(i));
				}

				if (approvalCount > 1) {
					onlyOneCandidate = false;
				}

				List<Integer> disapprovals;
				if (!useApprovalInterval) {
					// in this case, there is a fixed percentage of candidates that can be approved
					disapprovals = ElectoralSystemUtils.determineApprovedCandidates(voter, inverseStrategy,
							currentDisapprovalRate);
				} else {
					// in this case, the number of approved candidates can vary because the approval
					// rate is calculated as the range from the best priority
					disapprovals = ElectoralSystemUtils.determineApprovedCandidatesByInterval(voter, inverseStrategy,
							currentDisapprovalRate);
				}
				for (int i = 0; i < disapprovals.size(); i++) {
					votes.set(i, votes.get(i) - disapprovals.get(i));
				}
			}

			List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes,
					remainingCandidates);

			// if there is a single winner and we do not want to use a second round.
			if (winnerIndices.size() == 1) {
				return winnerIndices.get(0);
			}

			// we reduce the approval rate by two and reduce the number of candidates that
			// can be approved by the voters
			currentApprovalRate /= 2;

			// switch to plurality vote in case we vote for only one candidate, or the
			// approval rate becomes too small.
			if (onlyOneCandidate || currentApprovalRate < 10e-6) {
				return pluralityVote(priorities);
			}
		} while (true);
	}

	private int combinedApprovalVotingUsingIntervalo(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		// marko parameters
		approvalRate = 0.3;
		disapprovalRate = 0.3;
		useApprovalInterval = true;

		double currentApprovalRate = approvalRate;
		double currentDisapprovalRate = disapprovalRate;

		WinnerStrategy inverseStrategy = winnerStrategy.getInverse();

		do {
			List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));
			boolean onlyOneCandidate = true;

			for (List<Double> voter : priorities) {
				List<Integer> approvals;
				if (!useApprovalInterval) {
					// in this case, there is a fixed percentage of candidates that can be approved
					approvals = ElectoralSystemUtils.determineApprovedCandidates(voter, winnerStrategy,
							currentApprovalRate);
				} else {
					// in this case, the number of approved candidates can vary because the approval
					// rate is calculated as the range from the best priority
					approvals = ElectoralSystemUtils.determineApprovedCandidatesByInterval(voter, winnerStrategy,
							currentApprovalRate);
				}
				int approvalCount = 0;
				for (int i = 0; i < approvals.size(); i++) {
					approvalCount += approvals.get(i);
					votes.set(i, votes.get(i) + approvals.get(i));
				}

				if (approvalCount > 1) {
					onlyOneCandidate = false;
				}

				List<Integer> disapprovals;
				if (!useApprovalInterval) {
					// in this case, there is a fixed percentage of candidates that can be approved
					disapprovals = ElectoralSystemUtils.determineApprovedCandidates(voter, inverseStrategy,
							currentDisapprovalRate);
				} else {
					// in this case, the number of approved candidates can vary because the approval
					// rate is calculated as the range from the best priority
					disapprovals = ElectoralSystemUtils.determineApprovedCandidatesByInterval(voter, inverseStrategy,
							currentDisapprovalRate);
				}
				for (int i = 0; i < disapprovals.size(); i++) {
					votes.set(i, votes.get(i) - disapprovals.get(i));
				}
			}

			List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes,
					remainingCandidates);

			// if there is a single winner and we do not want to use a second round.
			if (winnerIndices.size() == 1) {
				return winnerIndices.get(0);
			}

			// we reduce the approval rate by two and reduce the number of candidates that
			// can be approved by the voters
			currentApprovalRate /= 2;

			// switch to plurality vote in case we vote for only one candidate, or the
			// approval rate becomes too small.
			if (onlyOneCandidate || currentApprovalRate < 10e-6) {
				return pluralityVote(priorities);
			}
		} while (true);
	}

	private int condorcetMethod(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
//        ((ArrayList<double[]>) rankings).ensureCapacity(voterCount);

		// determine the rankings
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		// determine the sum matrix between all the candidates
		List<List<Integer>> sumMatrix = ElectoralSystemUtils.calculateCondorcetMatrix(rankings);

		// determine whether there is a Condorcet winner.
		// i.e. whether there exists a candidate that received more preferences than any
		// other in pairwise comparison
		int condorcetWinnerIndex = ElectoralSystemUtils.determineCondorcetWinner(sumMatrix);
		if (condorcetWinnerIndex != -1) {
			return condorcetWinnerIndex;
		}
		// if there is no winner, we fall back to Borda count
		return bordaCount(priorities, 0); // standard
	}

	private int contingenVoting(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

//        ((ArrayList<List<Double>>) rankings).ensureCapacity(voterCount);

		LowestPriorityStrategy lowestPriorityStrategy = new LowestPriorityStrategy();

		// first determine the rankings
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		// we calculate the votes based on the first preferences
		for (int i = 0; i < rankings.size(); i++) {
			int winner = ElectoralSystemUtils.determineVote(rankings.get(i), lowestPriorityStrategy);
			votes.set(winner, votes.get(winner) + 1);
		}

		// we now check if we have a single winner and they have the majority
		List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes, remainingCandidates);
		if (winnerIndices.size() == 1
				&& ElectoralSystemUtils.hasMajority(votes, winnerIndices.get(0), priorities.size())) {
			return winnerIndices.get(0);
		}

		// if there was only a single winner in the first round, we have to find the
		// second one as well and add them for consideration
		if (winnerIndices.size() == 1) {
			List<Integer> secondBestIndices = ElectoralSystemUtils.determineSecondBestCandidates(votes,
					winnerIndices.get(0));
			winnerIndices.addAll(secondBestIndices);
		}

		// if we don't, we reset the votes and consider only the two best candidates
		votes.replaceAll(vote -> 0.0);

		for (int i = 0; i < rankings.size(); i++) {
			int winner = ElectoralSystemUtils.determineVoteFromSubset(rankings.get(i), lowestPriorityStrategy,
					winnerIndices);
			votes.set(winner, votes.get(winner) + 1);
		}

		return winnerIndices.get(0);
	}

	private int coombsMethod(List<List<Double>> priorities) {
		WinnerStrategy highest = new HighestPriorityStrategy();
		WinnerStrategy lowest = new LowestPriorityStrategy();

		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		WinnerStrategy inverseStrategy = winnerStrategy.getInverse();
//	        ((ArrayList<List<Double>>) rankings).ensureCapacity(voterCount);

		// calculate the rankings of all candidates
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		do {
			List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

			// determine the votes for the candidates by considering only the top choice
			for (List<Double> voter : rankings) {
				int winner = ElectoralSystemUtils.determineVoteFromSubset(voter, lowest, remainingCandidates);
				votes.set(winner, votes.get(winner) + 1);
			}

			List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes,
					remainingCandidates);

			// If there is only one candidate that received the most votes and they also
			// received the majority of them
			if (winnerIndices.size() == 1
					&& ElectoralSystemUtils.hasMajority(votes, winnerIndices.get(0), rankings.size())) {
				return winnerIndices.get(0);
			}

			// we now count the votes for the losers, however, we determine the loser by the
			// votes where the candidate came last
			List<Double> looserVotes = new ArrayList<>(decisionCount);
			for (int i = 0; i < decisionCount; i++) {
				looserVotes.add(0.0);
			}
			for (List<Double> voter : rankings) {
				int looser = ElectoralSystemUtils.determineVoteFromSubset(voter, highest, remainingCandidates);
				looserVotes.set(looser, looserVotes.get(looser) + 1);
			}

			List<Integer> looserIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(looserVotes,
					remainingCandidates);

			// if there are the same number of losers as the remaining candidates, it means
			// we have a tie
			if (remainingCandidates.size() == looserIndices.size() || remainingCandidates.size() == 1) {
				return winnerIndices.get(0);
			}

			// remove the losers
			remainingCandidates.removeAll(looserIndices);

		} while (true);
	}

	private int copelandMethod(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<Double> winner = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

//        ((ArrayList<List<Double>>) rankings).ensureCapacity(voterCount);

		// calculate all the rankings
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		List<List<Integer>> sumMatrix = ElectoralSystemUtils.calculateCondorcetMatrix(rankings);

		// check if there is a Condorcet winner, and if yes, end the method

		// if not, determine the Copeland score for each candidate
		List<List<Double>> preferenceMatrix = ElectoralSystemUtils.calculateSmithPreferenceMatrix(rankings);
		List<Double> copelandScore = ElectoralSystemUtils.calculateCopelandScoreFromPreferenceMatrix(preferenceMatrix,
				remainingCandidates);

		// determine the candidates with the highest Copeland score
		List<Integer> winners = ElectoralSystemUtils.getIndicesWithHighestVoteCount(copelandScore, remainingCandidates);

		List<Double> total = new ArrayList<>(decisionCount);
		for (int i = 0; i < decisionCount; i++) {
			total.add(0.0);
		}

		// if we have a tie, use Borda to resolve them
		if (winners.size() > 1) {
			for (List<Double> ranking : rankings) {
				for (int j = 0; j < winners.size(); j++) {
					total.set(winners.get(j), total.get(winners.get(j)) + decisionCount - ranking.get(winners.get(j)));
				}
			}

			List<Integer> newWinnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(total, winners);
			return newWinnerIndices.get(0);
		}

		return winners.get(0);
	}

	private int distributedVoting(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		do {
			List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));
			List<List<Double>> normalisedPriorities = new ArrayList<>();
//            ((ArrayList<List<Double>>) normalisedPriorities).ensureCapacity(voterCount);

			for (List<Double> voter : priorities) {
				normalisedPriorities
						.add(ElectoralSystemUtils.normaliseScores(voter, winnerStrategy, 100, 0, remainingCandidates));
			}

			// calculate the votes for all candidates
			for (List<Double> voter : normalisedPriorities) {
				for (int candidate : remainingCandidates) {
					votes.set(candidate, votes.get(candidate) + voter.get(candidate));
				}
			}

			List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes,
					remainingCandidates);

			// If there is only one candidate which received the most votes and he also
			// received the majority of them
			// if (winnerIndices.size() == 1 && ElectoralSystemUtils::hasMajority(votes,
			// winnerIndices[0], normalisedpriorities.size()))
			// {
			// return winnerIndices[0];
			// }

			// determine the losers, i.e. the candidates that obtained the lowest number of
			// votes
			List<Integer> loserIndices = ElectoralSystemUtils.getIndicesWithLowestVoteCount(votes, remainingCandidates);

			if (winnerIndices.isEmpty()) // ojo
				return loserIndices.get(0);

			// if the number of losers is the same as the number of remaining candidates,
			// then we have a tie that we cannot resolve and
			// return the first winner
			if ((remainingCandidates.size() == loserIndices.size() || remainingCandidates.size() == 1)) {
				return winnerIndices.get(0);
			}

			// remove losers from the remaining candidate list
			for (int loserIndex : loserIndices) {
				remainingCandidates.removeIf(candidate -> candidate == loserIndex);
			}

		} while (true);
	}

	private int instantRunoff(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		do {

			List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

			// calculate the votes for all candidates
			for (List<Double> voter : priorities) {
				int winner = ElectoralSystemUtils.determineVoteFromSubset(voter, winnerStrategy, remainingCandidates);
				votes.set(winner, votes.get(winner) + 1);
			}

			List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes,
					remainingCandidates);

			// If there is only one candidate which received most votes and he also received
			// the majority of them
			if (winnerIndices.size() == 1
					&& ElectoralSystemUtils.hasMajority(votes, winnerIndices.get(0), priorities.size())) {
				return winnerIndices.get(0);
			}

			// determine the losers, i.e., the candidates that obtained the lowest number of
			// votes
			List<Integer> loserIndices = ElectoralSystemUtils.getIndicesWithLowestVoteCount(votes, remainingCandidates);

			// if the number of losers is the same as the number of remaining candidates,
			// then we have a tie that we cannot resolve
			// and return the first winner
			if (remainingCandidates.size() == loserIndices.size() || remainingCandidates.size() == 1) {
				return winnerIndices.get(0);
			}

			// remove losers from the remaining candidate list
			remainingCandidates.removeAll(loserIndices);

		} while (true);
	}

	// marko parameters
	int minimumRating;
	int maximumRating;
	MedianRule medianRule;
	
    public int judgementMethod(List<List<Double>> priorities) {
	        List<List<Double>> totalScores = new ArrayList<>();
	        int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
	        int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
	        List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

	        // calculate the scores each voter gives to each candidate
	        for (int i = 0; i < priorities.size(); i++) {
	            List<Double> decisionScores = ElectoralSystemUtils.calculateScoresUniform(
	                    priorities.get(i), winnerStrategy, maximumRating, minimumRating);
	            totalScores.add(decisionScores);
	        }

	        // reorder the scores so that we get a list of scores for each candidate
	        List<List<Double>> scoresPerCandidate = new ArrayList<>();
	        for (int i = 0; i < priorities.get(0).size(); i++) {
	            List<Double> candidateScores = new ArrayList<>();
	            for (int j = 0; j < priorities.size(); j++) {
	                candidateScores.add(totalScores.get(j).get(i));
	            }
	            scoresPerCandidate.add(candidateScores);
	        }

	        // determine the median score for each candidate
	        List<Double> medians = new ArrayList<>();
	        for (List<Double> scores : scoresPerCandidate) {
	            medians.add(ElectoralSystemUtils.calculateMedian(scores));
	        }

	        // if there is only one candidate that obtained the highest median score, select him
	        List<Integer> winners = ElectoralSystemUtils.getIndicesWithHighestVoteCount(medians, remainingCandidates);
	        if (winners.size() == 1) {
	            return winners.get(0);
	        }

	        // if not, use a median strategy to decide the winner
	        List<Double> numberOfProponents = new ArrayList<>(scoresPerCandidate.size());
	        List<Double> numberOfOpponents = new ArrayList<>(scoresPerCandidate.size());

	        // calculate the number of proponents and opponents, i.e., the number of voters that
	        // gave a score larger than the median and a score lower than the median, respectively
	        for (int i = 0; i < scoresPerCandidate.size(); i++) {
	            double median = medians.get(i);
	            double proponents = 0;
	            double opponents = 0;
	            for (int j = 0; j < scoresPerCandidate.get(i).size(); j++) {
	                double candidateScore = scoresPerCandidate.get(i).get(j);
	                if (candidateScore < median) {
	                    opponents++;
	                } else if (candidateScore > median) {
	                    proponents++;
	                }
	            }
	            numberOfOpponents.add(opponents);
	            numberOfProponents.add(proponents);
	        }

	        // normalize the counts by the number of voters
	        double scoreCount = scoresPerCandidate.get(0).size();
	        for (int i = 0; i < numberOfProponents.size(); i++) {
	            numberOfOpponents.set(i, numberOfOpponents.get(i) / scoreCount);
	            numberOfProponents.set(i, numberOfProponents.get(i) / scoreCount);
	        }

	        List<Double> scores = new ArrayList<>(scoresPerCandidate.size());
	        // use a certain median rule for tie breaking
	        if (medianRule == MedianRule.TypicalJudgement) {
	            for (int i = 0; i < scoresPerCandidate.size(); i++) {
	                scores.add(medians.get(i) + numberOfProponents.get(i) - numberOfOpponents.get(i));
	            }
	        } else if (medianRule == MedianRule.UsualJudgement) {
	            for (int i = 0; i < scoresPerCandidate.size(); i++) {
	                scores.add(medians.get(i) + 0.5 * (numberOfProponents.get(i) - numberOfOpponents.get(i)) /
	                        (1.0 - numberOfProponents.get(i) - numberOfOpponents.get(i)));
	            }
	        } else if (medianRule == MedianRule.CentralJudgement) {
	            for (int i = 0; i < scoresPerCandidate.size(); i++) {
	                // add a small epsilon to ensure division by zero does not occur
	                scores.add(medians.get(i) + 0.5 * (numberOfProponents.get(i) - numberOfOpponents.get(i)) /
	                        (numberOfProponents.get(i) + numberOfOpponents.get(i) + 0.0000001));
	            }
	        } else if (medianRule == MedianRule.MajorityJudgement) {
	            for (int i = 0; i < scoresPerCandidate.size(); i++) {
	                scores.add(medians.get(i) + (numberOfProponents.get(i) > numberOfOpponents.get(i) ?
	                        numberOfProponents.get(i) : -numberOfOpponents.get(i)));
	            }
	        }

	        // the one with the highest score is the winner, in case of ties select the one with the lowest index
	        List<Integer> highestMedianIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(scores, remainingCandidates);
	        return highestMedianIndices.get(0);
	    }

	/*
	private int judgementMethod(List<List<Double>> priorities, int mode) {
		List<List<Double>> totalScores = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		// calculate the scores each voter gives to each candidate
		for (List<Double> priority : priorities) {
			List<Double> decisionScores = ElectoralSystemUtils.calculateScoresUniform(priority, winnerStrategy, maximumRating, minimumRating);
			totalScores.add(decisionScores);
		}

		// reorder the scores so that we have a list of scores for each candidate
		List<List<Double>> scoresPerCandidate = new ArrayList<>();
		for (int i = 0; i < priorities.get(0).size(); i++) {
			List<Double> candidateScores = new ArrayList<>();
			for (int j = 0; j < priorities.size(); j++) {
				candidateScores.add(totalScores.get(j).get(i));
			}
			scoresPerCandidate.add(candidateScores);
		}

		// determine the median score for each candidate
		List<Double> medians = new ArrayList<>();
		for (List<Double> scores : scoresPerCandidate) {
			medians.add(ElectoralSystemUtils.calculateMedian(scores));
		}

		// if there is only one candidate that obtained the highest median score, select
		// them
		List<Integer> winners = ElectoralSystemUtils.getIndicesWithHighestVoteCount(medians, remainingCandidates);
		if (winners.size() == 1) {
			return winners.get(0);
		}

		// calculate the number of proponents and opponents for each candidate
		List<Double> numberOfProponents = new ArrayList<>(scoresPerCandidate.size());
		List<Double> numberOfOpponents = new ArrayList<>(scoresPerCandidate.size());
		int scoreCount = scoresPerCandidate.get(0).size();
		for (int i = 0; i < scoresPerCandidate.size(); i++) {
			double median = medians.get(i);
			double proponents = 0;
			double opponents = 0;
			for (int j = 0; j < scoresPerCandidate.get(i).size(); j++) {
				double candidateScore = scoresPerCandidate.get(i).get(j);
				if (candidateScore < median) {
					opponents++;
				} else if (candidateScore > median) {
					proponents++;
				}
			}
			numberOfProponents.add(proponents / scoreCount);
			numberOfOpponents.add(opponents / scoreCount);
		}

		// calculate the scores using the specified median rule for tie-breaking
		List<Double> scores = new ArrayList<>(scoresPerCandidate.size());
		for (int i = 0; i < scoresPerCandidate.size(); i++) {
			scores.add(medianRule(medians.get(i), numberOfProponents.get(i), numberOfOpponents.get(i), mode));
		}

		// the candidate with the highest score is the winner; in case of ties, select
		// the one with the lowest index
		List<Integer> highestMedianIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(scores,
				remainingCandidates);
		return highestMedianIndices.get(0);
	}

	protected double medianRule(double medians, double numberOfProponents, double numberOfOpponents, int mode) {
		if (mode == 0) // central
			return medians + 0.5 * (numberOfProponents - numberOfOpponents)
					/ (numberOfProponents + numberOfOpponents + 0.000001);
		else if (mode == 1) // majority
			return medians + (numberOfProponents > numberOfOpponents ? numberOfProponents : -numberOfOpponents);
		else if (mode == 2) // typical
			return medians + numberOfProponents - numberOfOpponents;
		else // usual
			return medians
					+ 0.5 * (numberOfProponents - numberOfOpponents) / (1.0 - numberOfProponents - numberOfOpponents);
	}
	*/
	

	// marko parameters
	int maximumConsideredCandidates = 5;

	private int kemenyYoungMethod(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);

		// determine the rankings of all candidates
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		// calculate the sum matrix of all pairs of candidates
		List<List<Integer>> sumMatrix = ElectoralSystemUtils.calculateCondorcetMatrix(rankings);

		// determine whether there is a Condorcet winner
		int condorcetWinnerIndex = ElectoralSystemUtils.determineCondorcetWinner(sumMatrix);
		if (condorcetWinnerIndex != -1) {
			return condorcetWinnerIndex;
		}

		List<Integer> order = new ArrayList<>();
		for (int i = 0; i < decisionCount; i++) {
			order.add(i);
		}

		int bestScore = 0;
		List<Integer> winner = new ArrayList<>();

		// if there are more than the maximum considered candidates, eliminate a set of
		// candidates
		// and proceed to the Kemeny-Young method
		while (order.size() > maximumConsideredCandidates) {
			List<Double> total = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

			// determine the scores for all candidates
			for (List<Double> ranking : rankings) {
				for (int candidateIndex : order) {
					total.set(candidateIndex, total.get(candidateIndex) + decisionCount - ranking.get(candidateIndex));
				}
			}

			// determine the candidates that received the lowest score based on the rankings
			List<Integer> loserIndices = ElectoralSystemUtils.getIndicesWithLowestVoteCount(total, order);
			if (loserIndices.size() == order.size()) {
				return order.get(0);
			}

			// update the ranks of the candidates to be eliminated
			for (int loser : loserIndices) {
				for (List<Double> ranking : rankings) {
					// determine the rank of the loser
					int loserRank = ranking.get(loser).intValue();
					for (int j = 0; j < order.size(); j++) {
						int currentOrder = order.get(j);
						if (currentOrder == loser) {
							ranking.set(currentOrder, (double) (decisionCount + 1));
						} else if (loserRank < ranking.get(currentOrder)) {
							ranking.set(currentOrder, ranking.get(currentOrder) - 1);
						}
					}
				}
			}

			order.removeAll(loserIndices);
		}

		do {
			int score = ElectoralSystemUtils.calculatePermutationScore(order, sumMatrix);
			if (score > bestScore) {
				winner.clear();
				winner.add(order.get(0));
			} else if (score == bestScore) {
				winner.add(order.get(0));
			}
		} while (nextPermutation(order));

		// in case of ties, select the first permutation
		return winner.get(0);
	}

	private boolean nextPermutation(List<Integer> order) {
		int n = order.size();
		int i = n - 2;
		while (i >= 0 && order.get(i) >= order.get(i + 1)) {
			i--;
		}
		if (i < 0) {
			return false;
		}

		int j = n - 1;
		while (order.get(i) >= order.get(j)) {
			j--;
		}

		Collections.swap(order, i, j);
		int left = i + 1;
		int right = n - 1;
		while (left < right) {
			Collections.swap(order, left, right);
			left++;
			right--;
		}

		return true;
	}

	private int majorityVote(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		while (true) {
			// determine the votes for the remaining candidates
			List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

			for (List<Double> voter : priorities) {
				int winner = ElectoralSystemUtils.determineVoteFromSubset(voter, winnerStrategy, remainingCandidates);
				votes.set(winner, votes.get(winner) + 1);
			}

			List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes,
					remainingCandidates);

			// If there is only one candidate that received the majority of votes
			if (winnerIndices.size() == 1
					&& ElectoralSystemUtils.hasMajority(votes, winnerIndices.get(0), priorities.size())) {
				return winnerIndices.get(0);
			}

			// if there is only a single winner, it means they have no majority, so we add
			// the second best candidate
			if (winnerIndices.size() == 1) {
				List<Integer> secondBestIndices = ElectoralSystemUtils.determineSecondBestCandidates(votes,
						winnerIndices.get(0));
				winnerIndices.addAll(secondBestIndices);
			}

			// check whether the number of winners is the same as the number of remaining
			// candidates
			// this means we did not eliminate any candidate and we have a tie that we
			// cannot solve
			if (winnerIndices.size() == remainingCandidates.size()) {
				return winnerIndices.get(0);
			}

			// update the candidates for the next round
			remainingCandidates = winnerIndices;
		}
	}

	private int miniMax(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		// determine the rankings
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		List<List<Integer>> sumMatrix = ElectoralSystemUtils.calculateCondorcetMatrix(rankings);
		List<Double> defeats = ElectoralSystemUtils.calculatePairwiseDefeatMargins(sumMatrix);
		List<Integer> winners = ElectoralSystemUtils.getIndicesWithLowestVoteCount(defeats, remainingCandidates);

		if (winners.size() > 1) {
			List<Double> secondRoundVotes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

			// perform the second round of voting with only the winners from the first round
			for (List<Double> voter : priorities) {
				int winnerIndex = ElectoralSystemUtils.determineVoteFromSubset(voter, winnerStrategy, winners);

				secondRoundVotes.set(winnerIndex, secondRoundVotes.get(winnerIndex) + 1);
			}

			List<Integer> newWinnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(secondRoundVotes,
					winners);

			// if the size is equal, it means we were unable to reduce the number of
			// candidates. Therefore just return the first one.
			if (newWinnerIndices.size() == winners.size()) {
				return winners.get(0);
			}

			winners = newWinnerIndices;
		}

		return winners.get(0);
	}

	private int nansonMethod(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		// if there is a single candidate, we can return him as the winner
		if (decisionCount == 1) {
			return 0;
		}

		// calculate the ranks for each candidate
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		do {
			int[] total = new int[decisionCount];

			// determine the scores for all candidates
			for (List<Double> ranking : rankings) {
				for (int candidateIndex : remainingCandidates) {
					total[candidateIndex] += decisionCount - ranking.get(candidateIndex);
				}
			}

			// calculate the average score
			double totalScore = 0.0;
			double numberOfScores = remainingCandidates.size();
			for (int remainingCandidate : remainingCandidates) {
				totalScore += total[remainingCandidate];
			}
			double averageScore = totalScore / numberOfScores;

			// determine the candidates that received scores below the average Borda score
			List<Integer> looserIndices = ElectoralSystemUtils.getIndicesBelowAverage(total, remainingCandidates,
					averageScore);
			if (looserIndices.size() == 0 || looserIndices.size() == remainingCandidates.size()) {
				return remainingCandidates.get(0);
			}

			// update the ranks of all those candidates that will be eliminated
			for (int looser : looserIndices) {
				for (int i = 0; i < rankings.size(); i++) {
					// determine the rank of the looser
					double looserRank = rankings.get(i).get(looser);
					for (int remainingCandidate : remainingCandidates) {
						if (remainingCandidate == looser) {
							rankings.get(i).set(remainingCandidate, (double) (decisionCount + 1));
						} else if (looserRank < rankings.get(i).get(remainingCandidate)) {
							rankings.get(i).set(remainingCandidate, rankings.get(i).get(remainingCandidate) - 1);
						}
					}
				}
			}
			for (int i = looserIndices.size() - 1; i >= 0; i--) {
				remainingCandidates.remove(looserIndices.get(i));
			}

		} while (remainingCandidates.size() > 1);

		return remainingCandidates.get(0);
	}

	private int oklahomaMethod(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<Integer> winer = new ArrayList<>(Collections.nCopies(decisionCount, 0));

		// calculate the ranks for each candidate
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		// determine the number of choices that can be performed by the voters
		// this depends on the number of candidates
		int numberOfChoices = 1;
		if (priorities.get(0).size() == 3 || priorities.get(0).size() == 4) {
			numberOfChoices = 2;
		} else if (priorities.get(0).size() > 5) {
			numberOfChoices = 3;
		}

		int iteration = 1;
		List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

		// we determine the required majority
		int majority = priorities.get(0).size() / 2 + 1;

		do {
			// we consider only the rank of the current iteration (1, 2, or 3)
			for (List<Double> ranks : rankings) {
				int index = ElectoralSystemUtils.determineDecisionWithRank(ranks, iteration);
				// here we scale the votes by the preference position
				votes.set(index, votes.get(index) + 1.0 / iteration);
			}

			// check if we have a single winner
			List<Integer> winners = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes, remainingCandidates);
			if (winners.size() == 1 && votes.get(winners.get(0)) >= majority) {
				return winners.get(0);
			}
			iteration++;
		} while (iteration <= numberOfChoices);

		List<Integer> winners = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes, remainingCandidates);
		return winners.get(0);
	}

	private int pluralityVote(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));
		votes.add(0.0);

		// we count the votes for each candidate
		for (List<Double> voter : priorities) {
			int winner = ElectoralSystemUtils.determineVote(voter, winnerStrategy);
			votes.set(winner, votes.get(winner) + 1);
		}

		// we determine the indices of the rules with the most votes
		List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes, remainingCandidates);
		return winnerIndices.get(0);
	}

	private int pluralityVote2round(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

		// we count the votes for each candidate
		for (List<Double> voter : priorities) {
			int winner = ElectoralSystemUtils.determineVote(voter, winnerStrategy);
			votes.set(winner, votes.get(winner) + 1);
		}

		// we determine the indices of the rules with the most votes
		List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes, remainingCandidates);

		// we repeat until there is only one winner left
		if (winnerIndices.size() > 1) {
			List<Double> secondRoundVotes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

			// we perform the second round of voting with only the winners from the first
			// round
			for (List<Double> voter : priorities) {
				int winnerIndex = ElectoralSystemUtils.determineVoteFromSubset(voter, winnerStrategy, winnerIndices);
				secondRoundVotes.set(winnerIndex, secondRoundVotes.get(winnerIndex) + 1);
			}

			List<Integer> newWinnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(secondRoundVotes,
					winnerIndices);

			// if the size is equal, it means we were unable to reduce the number of
			// candidates. Therefore just return the first one.
			if (newWinnerIndices.size() == winnerIndices.size()) {
				return winnerIndices.get(0);
			}
			winnerIndices = newWinnerIndices;
		}
		return winnerIndices.get(0);
	}

	private int preferentialBlockVoting(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));
		List<List<Double>> rankings = new ArrayList<>(voterCount);

		// first calculate the ranks for all candidates and also count the first
		// preferences for each of them
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
			int winner = ElectoralSystemUtils.determineVote(voter, winnerStrategy);
			votes.set(winner, votes.get(winner) + 1);
		}

		int majority = (voterCount / 2) + 1;

		do {
			// we check if there is a single winner (candidate with the most votes) and if
			// they have a majority. If yes, we proclaim them the winner
			List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes,
					remainingCandidates);
			if (winnerIndices.size() == 1 && votes.get(winnerIndices.get(0)) >= majority) {
				return winnerIndices.get(0);
			}

			// we determine the losers, the candidates that received the lowest number of
			// first preferences
			List<Integer> losers = ElectoralSystemUtils.getIndicesWithLowestVoteCount(votes, remainingCandidates);

			// in case the number of losers is equal to the number of remaining candidates,
			// we have a tie that we cannot resolve.
			if (losers.size() == remainingCandidates.size()) {
				return winnerIndices.get(0);
			}

			// we then iterate through all losers to update the ranks
			for (int loser : losers) {
				for (List<Double> ranking : rankings) {
					// first, we determine the rank of the loser that we eliminate
					int loserRank = ranking.get(loser).intValue();
					for (int remainingCandidate : remainingCandidates) {
						if (remainingCandidate == loser) {
							ranking.set(remainingCandidate, (double) decisionCount + 1);
						} else if (loserRank < ranking.get(remainingCandidate).intValue()) {
							ranking.set(remainingCandidate, ranking.get(remainingCandidate) - 1);
						}
					}
				}
				// and finally we eliminate the loser from the remaining candidates.
				remainingCandidates.removeIf(candidate -> candidate == losers.get(0));
			}

			for (List<Double> ranking : rankings) {
				int winner = ElectoralSystemUtils.determineVoteFromSubset(ranking, winnerStrategy, remainingCandidates);
				votes.set(winner, votes.get(winner) + 1);
			}

		} while (true);
	}

	private int rankedPairs(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<Integer> winner = new ArrayList<>(Collections.nCopies(decisionCount, 0));
//        rankings.reserve(voterCount); ????

		// determine the ranks of all candidates
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		// first determine the tally. the tally denotes for each pair of candidates x
		// and y the number of votes where x was preferred over y
		// the tally is determined only for positive preferences
		List<List<Integer>> tally = ElectoralSystemUtils.calculateTallyMatrix(rankings);
		List<Triple<Integer, Integer, Integer>> preferences = new ArrayList<>();

		for (int i = 0; i < tally.size(); i++) {
			for (int j = i + 1; j < tally.size(); j++) {
				// if i wins over j, then we add it to the tally
				if (tally.get(i).get(j) >= 0) {
					Triple<Integer, Integer, Integer> tuple = new Triple<>(i, j, tally.get(i).get(j));
					preferences.add(tuple);
				} // otherwise we just reverse the indices since in that case j received more
					// preferences
				else {
					Triple<Integer, Integer, Integer> tuple = new Triple<>(j, i, tally.get(j).get(i));
					preferences.add(tuple);
				}
			}
		}

		// we sort the list of preferences from higher to lower
		Collections.sort(preferences, (t1, t2) -> t2.getThird() - t1.getThird());

		// then we determine the lock matrix.
		List<List<Integer>> lockMatrix = new ArrayList<>(decisionCount);
		for (int i = 0; i < decisionCount; i++) {
			List<Integer> row = new ArrayList<>(decisionCount);
			for (int j = 0; j < decisionCount; j++) {
				row.add(0);
			}
			lockMatrix.add(row);
		}

		// we set a 1 if for each pair when x is better than y, and -1 in the opposite
		// case
		for (Triple<Integer, Integer, Integer> preference : preferences) {
			// however, we only update those values that are not already set!
			// if the current place in the matrix is set we skip it
			if (lockMatrix.get(preference.getFirst()).get(preference.getSecond()) == 0) {
				// set the values
				lockMatrix.get(preference.getFirst()).set(preference.getSecond(), 1);
				lockMatrix.get(preference.getSecond()).set(preference.getFirst(), -1);

				// here we check for transitivity. we update also all those places where the one
				// to which we compared
				// was better than other choices
				for (int i = 0; i < decisionCount; i++) {
					if (lockMatrix.get(preference.getSecond()).get(i) == 1) {
						lockMatrix.get(preference.getFirst()).set(i, 1);
						lockMatrix.get(i).set(preference.getFirst(), -1);
					}
				}
			}
		}

		// we now go through the matrix and count the number of locks for each row
		List<Double> count = new ArrayList<>(decisionCount);
		for (int i = 0; i < lockMatrix.size(); i++) {
			Double rowSum = 0.0;
			for (int j = 0; j < lockMatrix.size(); j++) {
				rowSum += lockMatrix.get(i).get(j);
			}
			count.add(rowSum);
		}

		// the winner is the candidate that has the most locks. it is possible that ties
		// occur.
		List<Integer> winners = ElectoralSystemUtils.getIndicesWithHighestVoteCount(count, remainingCandidates);

		return winners.get(0);
	}

	private int schulzeMethod(List<List<Double>> priorities) {
		List<List<Double>> rankings = new ArrayList<>();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		// First determine the ranks of all candidates
		rankings = new ArrayList<>(voterCount);
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		// Determine the sum matrix and based on it determine the strength matrix
		List<List<Integer>> sumMatrix = ElectoralSystemUtils.calculateCondorcetMatrix(rankings);
		List<List<Integer>> strengthMatrix = ElectoralSystemUtils.calculateStrongestPaths(sumMatrix);

		// Based on the strength matrix, determine the number of wins of each candidate
		List<Double> wins = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

		for (int i = 0; i < decisionCount; i++) {
			for (int j = i + 1; j < decisionCount; j++) {
				if (strengthMatrix.get(i).get(j) > strengthMatrix.get(j).get(i)) {
					wins.set(i, wins.get(i) + 1);
				} else {
					wins.set(j, wins.get(j) + 1);
				}
			}
		}

		// Select the candidate with the most wins
		List<Integer> winners = ElectoralSystemUtils.getIndicesWithHighestVoteCount(wins, remainingCandidates);

		return winners.get(0);
	}

	// marko parameters
	int maxScore = 10;
	int minScore = 1;

	private int scoreVoting(List<List<Double>> priorities) {
		
		maxScore = SEC_adicionalSCORE.maxScore;
		minScore = SEC_adicionalSCORE.minScore;
		
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		List<Double> totalScores = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

		// Each voter scores all the candidates
		for (List<Double> voter : priorities) {
			// Calculate the score. We divide the interval of priorities (determined by the
			// minimum and maximum values obtained)
			// into a fixed number of subintervals. Based on which subinterval a certain
			// priority value belongs to,
			// we assign a corresponding score.
			List<Double> decisionScores = ElectoralSystemUtils.calculateScoresUniform(voter, winnerStrategy, maxScore,
					minScore);

			// Add the scores to each candidate
			for (int i = 0; i < decisionScores.size(); i++) {
				totalScores.set(i, totalScores.get(i) + decisionScores.get(i));
			}
		}

		// Determine the winners
		List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(totalScores,
				remainingCandidates);
		if (winnerIndices.size() == 1) {
			return winnerIndices.get(0);
		}

		// If we have a tie, use plurality to decide whom to select
		List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

		// First determine the rankings
		for (List<Double> voter : priorities) {
			// Here we immediately determine the votes in case there is a tie after the
			// Borda count
			int winner = ElectoralSystemUtils.determineVoteFromSubset(voter, winnerStrategy, winnerIndices);
			votes.set(winner, votes.get(winner) + 1);
		}

		List<Integer> newWinnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes, winnerIndices);
		return newWinnerIndices.get(0);
	}

	private int starVoting(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		// If there is only a single candidate
		if (priorities.get(0).size() == 1) {
			return 0;
		}

		List<Double> totalScores = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

		// First calculate the scores (by uniformly dividing the interval) and aggregate
		// them for each candidate
		for (List<Double> voter : priorities) {
			List<Double> decisionScores = ElectoralSystemUtils.calculateScoresUniform(voter, winnerStrategy, 5, 0);
			for (int i = 0; i < decisionScores.size(); i++) {
				totalScores.set(i, totalScores.get(i) + decisionScores.get(i));
			}
		}

		List<List<Double>> rankings = new ArrayList<>();

		// We also need to determine the ranks to be able to determine the preference
		// matrix
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		// Determine the winner and calculate the preference matrix
		List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(totalScores,
				remainingCandidates);
		List<List<Double>> preferenceMatrix = ElectoralSystemUtils.calculatePreferenceMatrix(rankings);

		// If there was no tie, find the candidates with the second best scores.
		if (winnerIndices.size() == 1) {
			List<Integer> secondBest = ElectoralSystemUtils.determineSecondBestCandidates(totalScores,
					winnerIndices.get(0));
			winnerIndices.addAll(secondBest);
		}

		// From the preference matrix, determine which of the two candidates is better
		if (preferenceMatrix.get(winnerIndices.get(0)).get(winnerIndices.get(1)) < preferenceMatrix
				.get(winnerIndices.get(1)).get(winnerIndices.get(0))) {
			return winnerIndices.get(0);
		}

		return winnerIndices.get(1);
	}

	private int tidemanAlternativeSmith(List<List<Double>> priorities) {
		WinnerStrategy highest = new HighestPriorityStrategy();
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		int voterCount = ElectoralSystemUtils.getNumberOfVoters(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);

		List<List<Double>> rankings = new ArrayList<>();
//	        rankings.reserve(voterCount); ????
		for (List<Double> voter : priorities) {
			rankings.add(ElectoralSystemUtils.calculateRanksUnique(voter, winnerStrategy));
		}

		List<List<Double>> preferenceMatrix = ElectoralSystemUtils.calculateSmithPreferenceMatrix(rankings);

		do {
			List<Double> copelandScore = ElectoralSystemUtils
					.calculateCopelandScoreFromPreferenceMatrix(preferenceMatrix, remainingCandidates);
			List<Integer> sortedIndices = ElectoralSystemUtils.sortByScores(remainingCandidates, copelandScore,
					highest);
			List<Integer> copelandWinners = ElectoralSystemUtils.getIndicesWithHighestVoteCount(copelandScore,
					remainingCandidates);
			int smithSetCardinality;
			List<Integer> smithSet = new ArrayList<>();
//	          smithSet.reserve(copelandWinners.size()); ????

			for (int i = 0; i < copelandWinners.size(); i++) {
				smithSet.add(sortedIndices.get(i));
			}

			do {
				smithSetCardinality = smithSet.size();
				for (int j = smithSetCardinality; j < remainingCandidates.size(); j++) {
					for (int i = 0; i < smithSetCardinality; i++) {
						int index = smithSet.get(i);
						int comparison = sortedIndices.get(j);
						if (preferenceMatrix.get(comparison).get(index) != 0) {
							smithSet.add(comparison);
							break;
						}
					}
				}
			} while (smithSet.size() > smithSetCardinality);

			if (smithSet.size() == 1) {
				return smithSet.get(0);
			}

			List<Double> votes = new ArrayList<>(decisionCount);
			for (int i = 0; i < decisionCount; i++) {
				votes.add(0.0);
			}

			// Determine the votes for the candidates where the candidate was ranked the
			// lowest
			for (List<Double> voter : rankings) {
				int winner = ElectoralSystemUtils.determineVoteFromSubset(voter, highest, smithSet);
				votes.set(winner, votes.get(winner) + 1);
			}

			List<Integer> loserIndices = ElectoralSystemUtils.getIndicesWithLowestVoteCount(votes, smithSet);
			remainingCandidates = smithSet;

			if (loserIndices.size() == smithSet.size()) {
				return smithSet.get(0);
			}

			// Remove losers from the remaining candidate list
			for (int looserIndex : loserIndices) {
				remainingCandidates.removeIf(candidate -> candidate == looserIndex);
			}
		} while (true);
	}

	private int unifiedPrimary(List<List<Double>> priorities) {
		int decisionCount = ElectoralSystemUtils.getNumberOfDecisions(priorities);
		List<Integer> remainingCandidates = ElectoralSystemUtils.createInitialCandidateList(decisionCount);
		List<Double> votes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

		for (List<Double> voter : priorities) {
			List<Integer> approvals;
			if (!useApprovalInterval) {
				approvals = ElectoralSystemUtils.determineApprovedCandidates(voter, winnerStrategy, approvalRate);
			} else {
				approvals = ElectoralSystemUtils.determineApprovedCandidatesByInterval(voter, winnerStrategy,
						approvalRate);
			}
			for (int i = 0; i < approvals.size(); i++) {
				votes.set(i, votes.get(i) + approvals.get(i));
			}
		}

		List<Integer> winnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(votes, remainingCandidates);

		if (winnerIndices.size() == 1) {
			List<Integer> secondBest = ElectoralSystemUtils.determineSecondBestCandidates(votes, winnerIndices.get(0));
			winnerIndices.addAll(secondBest);
		}

		List<Double> secondRoundVotes = new ArrayList<>(Collections.nCopies(decisionCount, 0.0));

		for (List<Double> voter : priorities) {
			int winnerIndex = ElectoralSystemUtils.determineVoteFromSubset(voter, winnerStrategy, winnerIndices);
			secondRoundVotes.set(winnerIndex, secondRoundVotes.get(winnerIndex) + 1);
		}

		List<Integer> newWinnerIndices = ElectoralSystemUtils.getIndicesWithHighestVoteCount(secondRoundVotes,
				winnerIndices);
		return newWinnerIndices.get(0);
	}
	
	private int suma(List<List<Double>> priorities) {
		int best = 0;
		double bestPriority = 0;
		double priority;
		for (int i = 0; i < priorities.size(); i++) {
			priority = 0.0;
			for (int j = 0; j < priorities.get(0).size(); j++) {
				if (!Double.isFinite(priorities.get(i).get(j)))
					priority += priorities.get(i).get(j);
			}
			if (priority > bestPriority)
				best = i;
		}
		return best;
	}

	// combination methods anteriores
	int suma(Intervalo intervalo) {
		int nReglas = ensemble.getSizeReal();
		double bestPriority = -Double.MAX_VALUE;
		double priority;
		double priorityRule;
		int best = -1;
		for (Integer tarea : sin_asignar) {
			priority = 0;
			for (int regla = 0; regla < nReglas; regla++) {
				if (intervalo.libre() && OperacionesAsignacionTrabajosMaquina.propaga(
						OperacionesAsignacionTrabajosMaquina.creaIntervalo(intervalo), (int) instancia.p[tarea],
						OperacionesAsignacionTrabajosMaquina.copiaIntervalos(intervalos))) {
					priorityRule = ensemble.get(regla).calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo);
					if (!Double.isFinite(priorityRule))
						priorityRule = 0;
					priority += priorityRule;
				}
			}
			if (priority > bestPriority) {
				bestPriority = priority;
				best = tarea;
			}
		}
		return best;
	}

	int votacionMarko(Intervalo intervalo) {
		int nReglas = ensemble.getSizeReal();
		double[][] votaciones = new double[nReglas][instancia.p.length];
		double bestPriority;
		double priorityRule;
		int best;
		// compute a mtrix with the priority of each job with each rule
		for (int regla = 0; regla < nReglas; regla++) {
			bestPriority = -Double.MAX_VALUE;
			best = 0;
			for (Integer tarea : sin_asignar) {
				votaciones[regla][tarea] = 0;
				if (intervalo.libre() && OperacionesAsignacionTrabajosMaquina.propaga(
						OperacionesAsignacionTrabajosMaquina.creaIntervalo(intervalo), (int) instancia.p[tarea],
						OperacionesAsignacionTrabajosMaquina.copiaIntervalos(intervalos))) {
					priorityRule = ensemble.get(regla).calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo);
					if (!Double.isFinite(priorityRule))
						priorityRule = 0;
					if (priorityRule > bestPriority) {
						bestPriority = priorityRule;
						best = tarea;
					}
				}

			}
			votaciones[regla][best] = 1; // the most priority has 1, others have 0
		}
		// like sum combination vote
		best = -1;
		bestPriority = -Double.MAX_VALUE;
		double priority;
		for (Integer tarea : sin_asignar) {
			priority = 0;
			for (int regla = 0; regla < nReglas; regla++)
				priority += votaciones[regla][tarea];
			if (priority > bestPriority) {
				bestPriority = priority;
				best = tarea;
			}
		}
		return best;
	}

	int votacionFran(Intervalo intervalo) {
		int nReglas = ensemble.getSizeReal();
		// se precalculan las prioridades
		double[][] prioridades = new double[nReglas][instancia.p.length];
		double priorityRule;
		for (int regla = 0; regla < nReglas; regla++)
			for (Integer tarea : sin_asignar)
				if (intervalo.libre() && OperacionesAsignacionTrabajosMaquina.propaga(
						OperacionesAsignacionTrabajosMaquina.creaIntervalo(intervalo), (int) instancia.p[tarea],
						OperacionesAsignacionTrabajosMaquina.copiaIntervalos(intervalos))) {
					priorityRule = ensemble.get(regla).calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo);
					if (!Double.isFinite(priorityRule))
						priorityRule = 0;
					prioridades[regla][tarea] = priorityRule;
				}
		// se calcula el ranking de votaciones
		double[][] votaciones = new double[nReglas][instancia.p.length];
		List<Integer> tareasVotadas = new ArrayList<Integer>();
		int nota;
		for (int regla = 0; regla < nReglas; regla++) {
			// la nota inicial es siempre cero
			nota = 0;
			tareasVotadas.clear();
			while (tareasVotadas.size() < sin_asignar.size()) {
				int mejorTarea = 0;
				// por lo tanto, vamos buscando las tareas menos prioritarias
				double min = Double.MAX_VALUE;
				for (Integer tarea : sin_asignar)
					if (!tareasVotadas.contains(tarea) && prioridades[regla][tarea] < min
							&& Double.isFinite(prioridades[regla][tarea])) {
						mejorTarea = tarea;
						min = prioridades[regla][tarea];
					}
				tareasVotadas.add(mejorTarea);
				votaciones[regla][mejorTarea] = nota;
				nota++; // se suma +1 la nota
			}
		}
		// una vez se tienen las notas se suman y se busca la más prioritaria
		double bestPriority = -Double.MAX_VALUE;
		int best = -1;
		prioridad = new double[instancia.p.length];
		for (Integer tarea : sin_asignar) {
			for (int regla = 0; regla < nReglas; regla++)
				prioridad[tarea] += votaciones[regla][tarea];
			if (bestPriority < prioridad[tarea]) {
				best = tarea;
				bestPriority = prioridad[tarea];
			}
		}
		return best;
	}

	int votacionMarko2rounds(Intervalo intervalo) {
		int nReglas = ensemble.getSizeReal();
		double[][] votaciones = new double[nReglas][instancia.p.length];
		double[][] prioridades = new double[nReglas][instancia.p.length]; // las guardamos para la seunda ronda
		double bestPriority;
		double priorityRule;
		int best;
		// compute a mtrix with the priority of each job with each rule
		for (int regla = 0; regla < nReglas; regla++) {
			bestPriority = -Double.MAX_VALUE;
			best = 0;
			for (Integer tarea : sin_asignar) {
				votaciones[regla][tarea] = 0;
				if (intervalo.libre() && OperacionesAsignacionTrabajosMaquina.propaga(
						OperacionesAsignacionTrabajosMaquina.creaIntervalo(intervalo), (int) instancia.p[tarea],
						OperacionesAsignacionTrabajosMaquina.copiaIntervalos(intervalos))) {
					priorityRule = ensemble.get(regla).calculaPrioridadDe(instancia, sin_asignar, tarea, intervalo);
					if (!Double.isFinite(priorityRule))
						priorityRule = 0;
					if (priorityRule > bestPriority) {
						bestPriority = priorityRule;
						best = tarea;
					}
					prioridades[regla][tarea] = priorityRule;
				} else {
					prioridades[regla][tarea] = Double.NaN;
				}

			}
			votaciones[regla][best] = 1; // the most priority has 1, others have 0
		}
		// like sum combination vote
		// hasta aqui era lo mismo que el otro
		// first round
		best = -1;
		int secondBest = -1;
		bestPriority = -Double.MAX_VALUE;
		double priority;
		for (Integer tarea : sin_asignar) {
			priority = 0;
			for (int regla = 0; regla < nReglas; regla++)
				priority += votaciones[regla][tarea];
			if (priority > bestPriority) {
				bestPriority = priority;
				secondBest = best;
				best = tarea;
			}
		}
		if (secondBest == -1)
			return best;
		// second round
		int votesFirst = 0;
		int votesSecond = 0;
		for (int regla = 0; regla < nReglas; regla++) {
			if (Double.valueOf(prioridades[regla][best]).isNaN()) {
				votesSecond++;
			} else if (Double.valueOf(prioridades[regla][secondBest]).isNaN()) {
				votesFirst++;
			} else if (prioridades[regla][best] >= prioridades[regla][secondBest]) {
				votesFirst++;
			} else {
				votesSecond++;
			}
		}
		if (votesFirst >= votesSecond)
			return best;
		else
			return secondBest;
	}

}