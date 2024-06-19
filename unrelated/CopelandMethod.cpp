#include "CopelandMethod.h"

#include <iostream>
#include <numeric>

#include "ElectoralSystemUtils.h"

int CopelandMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<int> winner(decisionCount, 0);
	rankings.reserve(voterCount);

	// calculate all the rankings
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	auto sumMatrix = ElectoralSystemUtils::calculateCondorcetMatrix(rankings);

	// check if there is a condorcet winner, and if yes end the method


	//if not, determine the copeland score for each candidate
	auto pereferenceMatrix = ElectoralSystemUtils::calculateSmithPreferencetMatrix(rankings);
	auto copelandScore = ElectoralSystemUtils::calculateCopelandScoreFromPreferenceMatrix(pereferenceMatrix, remainingCandidates);
	// determine the candidated with the highest copeland score
	auto winners = ElectoralSystemUtils::getIndicesWithHighestVoteCount(copelandScore, remainingCandidates);

	vector<int> total(decisionCount, 0.0);

	// if we have a tie, use borda to resolve them
	if(winners.size()>1)
	{
		for (auto& ranking : rankings)
		{
			for (int j = 0; j < winners.size(); j++)
			{
				total[winners[j]] += decisionCount - (ranking[winners[j]]);
			}
		}

		auto newWinnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(total, winners);
		return newWinnerIndices[0];
	}


	return winners[0];
}
