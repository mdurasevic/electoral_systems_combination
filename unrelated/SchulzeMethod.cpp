#include "SchulzeMethod.h"
#include "ElectoralSystemUtils.h"

int SchulzeMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);

	
	// first deterine the ranks of all candidates
	rankings.reserve(voterCount);
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	// determne the sum matrix and based on it determine the strength matrix
	auto sumMatrix = ElectoralSystemUtils::calculateCondorcetMatrix(rankings);
	auto strengthMatrix = ElectoralSystemUtils::calculateStrongestPaths(sumMatrix);

	// based on the strength matrix, we then determine the number of wins of each candidate 
	std::vector<int> wins(decisionCount, 0);

	for(int i = 0; i<decisionCount; i++)
	{
		for(int j = i+1; j<decisionCount; j++)
		{
			if(strengthMatrix[i][j]>strengthMatrix[j][i])
			{
				wins[i] += 1;
			} else
			{
				wins[j] += 1;
			}
		}
	}

	// the candidate with the most wins is selected
	auto winners = ElectoralSystemUtils::getIndicesWithHighestVoteCount(wins, remainingCandidates);

	return winners[0];
}
