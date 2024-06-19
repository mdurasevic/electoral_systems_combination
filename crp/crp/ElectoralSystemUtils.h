#pragma once
#include <memory>
#include <vector>

#include "WinnerStrategy.h"

class ElectoralSystemUtils
{
public:
	static int determineVote(std::vector<double>& priorities, std::shared_ptr<WinnerStrategy> winnerStrategy);
	static int determineVoteFromSubset(std::vector<double>& priorities, std::shared_ptr<WinnerStrategy> winnerStrategy, std::vector<int>& candidates);
	template<typename T> static std::vector<int> getIndicesWithHighestVoteCount(std::vector<T>& votes, std::vector<int>& candidateIndices);
	static bool hasMajority(std::vector<int>& votes, int candidate, int voterCount);
	static std::vector<int> determineSecondBestCandidates(std::vector<int>& votes, int winnerIndex);
	template<typename T> static std::vector<int> getIndicesWithLowestVoteCount(std::vector<T>& votes, std::vector<int>& candidateIndices);
	static std::vector<int> getIndicesBelowAverage(std::vector<int>& scores, std::vector<int>& candidateIndices,
	                                               double averageScore);
	static std::vector<double> calculateRanksFractional(std::vector<double>& priorities, std::shared_ptr<WinnerStrategy> winnerStrategy);
	static std::vector < double > calculateRanksUnique(std::vector<double>& priorities, std::shared_ptr<WinnerStrategy> winnerStrategy);
	static std::vector<std::vector<int>> calculateCondorcetMatrix(std::vector<std::vector<double>>& ranks);
	static int determineCondorcetWinner(std::vector<std::vector<int>>& sumMatrix);
	static std::vector<std::vector<int>> calculateStrongestPaths(std::vector<std::vector<int>>& sumMatrix);
	static std::vector<std::vector<int>> calculateTallyMatrix(std::vector<std::vector<double>>& ranks);
	static std::vector<int> calculateCopelandScore(std::vector<std::vector<double>>& ranks);
	static std::vector<double> calculateCopelandScoreFromPreferenceMatrix(std::vector<std::vector<double>>& preferences, std::vector<int>& candidates);
	static int calculatePermutationScore(std::vector<int>& order, std::vector<std::vector<int>>& preferences);
	static std::vector<int> calculatePairwiseDefeats(std::vector<std::vector<int>>& sumMatrix);
	static std::vector<int> calculatePairwiseDefeatMargins(std::vector<std::vector<int>>& sumMatrix);
	static std::vector<int> calculatePairwiseOpposition(std::vector<std::vector<int>>& sumMatrix);
	static int determineDecisionWithRank(std::vector<double>& rankings, int rank);
	static std::vector<int> calculateScoresUniform(std::vector<double>& priorities, std::shared_ptr<WinnerStrategy> winnerStrategy, int maximumScore, int minimumScore);
	static std::vector<double> calculateScoresUniform(std::vector<double>& voter, std::shared_ptr<WinnerStrategy> winnerStrategy,
	                                        int maximumScore, int minimumScore, std::vector<int> candidateIndices);
	static std::vector<double> normaliseScores(std::vector<double>& voter, std::shared_ptr<WinnerStrategy> winnerStrategy,
	                                    int maximumScore, int minimumScore, std::vector<int> candidateIndices);
	static std::vector<int> determineApprovedCandidates(std::vector<double>& priorities, std::shared_ptr<WinnerStrategy> winnerStrategy, double approvalRate);
	static std::vector<int> determineApprovedCandidatesByInterval(std::vector<double>& priorities, std::shared_ptr<WinnerStrategy> winnerStrategy,double approvalRate);
	static std::vector<std::vector<int>> calculatePreferencetMatrix(std::vector<std::vector<double>>& ranks);
	static int calculateMedian(std::vector<int>& scores);
	static int getNumberOfDecisions(std::vector<std::vector<double>> priorities);
	static int getNumberOfVoters(std::vector<std::vector<double>> priorities);
	static std::vector<int> createInitialCandidateList(int numberOfCandidates);
	static std::vector<std::vector<double>> calculateSmithPreferencetMatrix(std::vector<std::vector<double>>& ranks);
	static std::vector<int> sortByScores(std::vector<int>& indices, std::vector<double>& scores, std::shared_ptr<WinnerStrategy> winnerStrategy);
};


