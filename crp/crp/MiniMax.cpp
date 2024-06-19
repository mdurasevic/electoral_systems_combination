#include "MiniMax.h"
#include "ElectoralSystemUtils.h"

int MiniMax::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);

	// determine the rankings
	rankings.reserve(voterCount);
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	auto sumMatrix = ElectoralSystemUtils::calculateCondorcetMatrix(rankings);
	auto defeats = ElectoralSystemUtils::calculatePairwiseDefeatMargins(sumMatrix);
	auto winners = ElectoralSystemUtils::getIndicesWithLowestVoteCount(defeats, remainingCandidates);

	if (winners.size() > 1)
	{
		vector<int> secondRoundVotes(decisionCount, 0);

		// we perform the second round of voting with only the winners from the first round
		for (auto& voter : priorities)
		{
			const int winnerIndex = ElectoralSystemUtils::determineVoteFromSubset(voter, winnerStrategy, winners);
			secondRoundVotes[winnerIndex]++;
		}

		auto newWinnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(secondRoundVotes, winners);

		// if the size is equal, it means we were unable to reduce the number of candidates. Therefore just return the first one.
		if (newWinnerIndices.size() == winners.size())
		{
			return winners[0];
		}
		winners = newWinnerIndices;
	}

	return winners[0];
}
