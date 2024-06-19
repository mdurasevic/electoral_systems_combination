#include "PluralityVote.h"
#include <numeric>
#include "ElectoralSystemUtils.h"


PluralityVote::PluralityVote(bool useSecondRound)
{
	this->useSecondRound = useSecondRound;
}

int PluralityVote::elect(vector<vector<double>> &priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<int> votes(decisionCount, 0);

	// we count the votes for each candidate
	for (auto& voter : priorities)
	{
		int winner = ElectoralSystemUtils::determineVote(voter, winnerStrategy);
		votes[winner]++;
	}

	//we determine the indices of the rules with the most votes
	vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, remainingCandidates);

	//we repeat until there is only one winner left
	if(winnerIndices.size()>1 && useSecondRound)
	{
		vector<int> secondRoundVotes(decisionCount, 0);

		// we perform the second round of voting with only the winners from the first round
		for(auto& voter : priorities)
		{
			const int winnerIndex = ElectoralSystemUtils::determineVoteFromSubset(voter, winnerStrategy, winnerIndices);
			secondRoundVotes[winnerIndex]++;
		}

		auto newWinnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(secondRoundVotes, winnerIndices);

		// if the size is equal, it means we were unable to reduce the number of candidates. Therefore just return the first one.
		if(newWinnerIndices.size()==winnerIndices.size())
		{
			return winnerIndices[0];
		}
		winnerIndices = newWinnerIndices;
	}

	return winnerIndices[0];
}
