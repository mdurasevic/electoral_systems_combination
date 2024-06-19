#include "CondorcetMethod.h"

#include <iostream>

#include "BordaCount.h"
#include "ElectoralSystemUtils.h"

int CondorcetMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	rankings.reserve(voterCount);
	
	// determine the rankings
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	// determine the sum matrix between all the candidates
	auto sumMatrix = ElectoralSystemUtils::calculateCondorcetMatrix(rankings);

	// determine whether there is a condorcet winner.
	//i.e. whether there exists a candidate that received more preferences than any other in pairwise comparison
	const int condorcetWinnerIndex = ElectoralSystemUtils::determineCondorcetWinner(sumMatrix);
	if (condorcetWinnerIndex != -1)
	{
		return condorcetWinnerIndex;
	}

	// if there is no winner, we fall back to borda count, this is black method
	BordaCount bordaCount(BordaCountStrategy::StandardBorda);

	return bordaCount.elect(priorities, winnerStrategy);
}
