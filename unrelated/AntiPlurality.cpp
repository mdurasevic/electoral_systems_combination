#include "AntiPlurality.h"

#include <numeric>

#include "ElectoralSystemUtils.h"
#include "HighestPriorityStrategy.h"

int AntiPlurality::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	auto highest = make_shared<HighestPriorityStrategy>();
	vector<vector<double>> rankings;
	rankings.reserve(voterCount);

	// calculate the rankings of all candidates
	for (auto& voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	do {
		vector<int> votes(decisionCount, 0);

		// determine the votes for the candidates where the candidate was ranked the lowest
		// largest rank means worst solution
		for (auto& voter : rankings)
		{
			const int winner = ElectoralSystemUtils::determineVoteFromSubset(voter, highest, remainingCandidates);
			votes[winner]++;
		}

		// we then determine the winners as those candidates that received the lowest number of votes against them
		vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithLowestVoteCount(votes, remainingCandidates);

		// If there is only one candidate that received the most votes against himself he is the winner (no majority required)
		if (winnerIndices.size() == 1)
		{
			return winnerIndices[0];
		}

		// if the number of winners is the same as the number of remaining candidates, we have a tie that we cannot resolve
		if(winnerIndices.size() == remainingCandidates.size())
		{
			return  winnerIndices[0];
		}

		remainingCandidates = std::move(winnerIndices);

	} while (true);
}
