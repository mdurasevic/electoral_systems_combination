#include "MajorityVote.h"

#include <numeric>

#include "ElectoralSystemUtils.h"

int MajorityVote::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);

	do {
		// determine the votes for the remaining candidates
		vector<int> votes(decisionCount, 0);
		for (auto& voter : priorities)
		{
			const int winner = ElectoralSystemUtils::determineVoteFromSubset(voter, winnerStrategy, remainingCandidates);
			votes[winner]++;
		}

		vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, remainingCandidates);
		// If there is only one candidate that received the majority of votes 
		if (winnerIndices.size() == 1 && ElectoralSystemUtils::hasMajority(votes, winnerIndices[0], priorities.size()))
		{
			return winnerIndices[0];
		}

		// if there is only a single winner, it means he has no majority, so we add the second best candidate
		if (winnerIndices.size() == 1)
		{
			auto secondBestIndices = ElectoralSystemUtils::determineSecondBestCandidates(votes, winnerIndices[0]);
			for (int secondBest : secondBestIndices)
			{
				winnerIndices.push_back(secondBest);
			}
		}

		// we check whether the number of the winners is the same as the number of the remaining candidates
		// this means we did not eliminate any candidate and we have a tie that we cannot solve
		if(winnerIndices.size() == remainingCandidates.size())
		{
			return winnerIndices[0];
		}

		// we just update the candidates for the next round
		remainingCandidates = winnerIndices;

	} while (true);

}
