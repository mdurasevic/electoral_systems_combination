#include "StarVoting.h"

#include <numeric>

#include "ElectoralSystemUtils.h"

int StarVoting::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);

	// if there is only a single candidate
	if(priorities[0].size() == 1)
	{
		return 0;
	}

	std::vector<int> totalScores(priorities[0].size(), 0);

	// first calculate the scores (by uniformly dividing the interval) and aggregate them for each candidate
	for (auto& voter : priorities)
	{
		auto decisionScores = ElectoralSystemUtils::calculateScoresUniform(voter, winnerStrategy, 5, 0);
		for (int i = 0; i < decisionScores.size(); i++)
		{
			totalScores[i] += decisionScores[i];
		}
	}

	vector<vector<double>> rankings;
	// we also need to determine the rankds to be able to determine the preference matrix
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	// determine the winner and calculate the preference matrix
	vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(totalScores, remainingCandidates);
	auto preferenceMatrix = ElectoralSystemUtils::calculatePreferencetMatrix(rankings);

	//if there was no tie, find the cancidates with the second best scores.
 	if(winnerIndices.size()==1)
	{
			vector<int> secondBest = ElectoralSystemUtils::determineSecondBestCandidates(totalScores, winnerIndices[0]);
			for (int second_best : secondBest)
			{
				winnerIndices.push_back(second_best);
			}
	}

	// from the preference matrix we determine which of the two candidates is better
	if(preferenceMatrix[winnerIndices[0]][winnerIndices[1]]<preferenceMatrix[winnerIndices[1]][winnerIndices[0]])
	{
		return winnerIndices[0];
	}

	return winnerIndices[1];
	
}
