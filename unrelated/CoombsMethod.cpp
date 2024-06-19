#include "CoombsMethod.h"

#include <numeric>

#include "ElectoralSystemUtils.h"
#include "HighestPriorityStrategy.h"
#include "LowestPriorityStrategy.h"

int CoombsMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	const auto inverseStrategy = winnerStrategy->getInverse();
	rankings.reserve(voterCount);

	// calculate the rankings of all candidates
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	do{
		vector<int> votes(decisionCount, 0);

		// determine the votes for the candidates by considering only the top choice
		for (auto& voter : rankings)
		{
			const int winner = ElectoralSystemUtils::determineVoteFromSubset(voter, lowest, remainingCandidates);
			votes[winner]++;
		}

		vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, remainingCandidates);

		// If there is only one candidate that received most votes and he also received the majority of them
		if (winnerIndices.size() == 1 && ElectoralSystemUtils::hasMajority(votes, winnerIndices[0], rankings.size()))
		{
			return winnerIndices[0];
		}

		// we now count the votes for the loosers, however we determine the looser by the votes where the candidate came at the last place
		vector<int> looserVotes(decisionCount, 0);
		for (auto& voter : rankings)
		{
			const int looser = ElectoralSystemUtils::determineVoteFromSubset(voter, highest, remainingCandidates);
			looserVotes[looser]++;
		}

		vector<int> looserIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(looserVotes,remainingCandidates);

		// if there is the same number of loosers as the remaning candidates, it means we have a tie
		if (remainingCandidates.size() == looserIndices.size() || remainingCandidates.size() == 1)
		{
			return winnerIndices[0];
		}

		// remove the loosers
		for (int looserIndex : looserIndices)
		{
			remainingCandidates.erase(std::remove(remainingCandidates.begin(), remainingCandidates.end(), looserIndex), remainingCandidates.end());
		}

	} while (true);

}
