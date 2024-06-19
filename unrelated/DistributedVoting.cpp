#include "DistributedVoting.h"

#include <numeric>

#include "ElectoralSystemUtils.h"

int DistributedVoting::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<int> votes(decisionCount, 0);

	do {
		vector<double> votes(decisionCount, 0);
		
		std::vector<std::vector<double>> normalisedPriorities;
		normalisedPriorities.reserve(voterCount);
		for (auto& voter : priorities)
		{
			normalisedPriorities.push_back(ElectoralSystemUtils::normaliseScores(voter, winnerStrategy, 100, 0, remainingCandidates));
		}

		// calculate the votes for all candidates
		for (auto& voter : normalisedPriorities)
		{
			for(auto candidate : remainingCandidates)
			{
				votes[candidate] += voter[candidate];
			}
		}

		vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, remainingCandidates);

		// If there is only one candidate which received most votes and he also received the majority of them
		//if (winnerIndices.size() == 1 && ElectoralSystemUtils::hasMajority(votes, winnerIndices[0], normalisedpriorities.size()))
		//{
		//	return winnerIndices[0];
		//}

		// determine the loosers, i.e. the candidates that obtained the lowest number of votes
		auto looserIndices = ElectoralSystemUtils::getIndicesWithLowestVoteCount(votes, remainingCandidates);

		// if the number of loosers is of the same size as the number of remaining candidates, then we have a tie that we cannot resolve and
		// return the first winner
		if (remainingCandidates.size() == looserIndices.size() || remainingCandidates.size() == 1)
		{
			return winnerIndices[0];
		}

		// remove loosers from the remaining candidate list
		for (const int looserIndex : looserIndices)
		{
			remainingCandidates.erase(std::remove(remainingCandidates.begin(), remainingCandidates.end(), looserIndex), remainingCandidates.end());
		}

	} while (true);
}
