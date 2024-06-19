#include "BucklinVoting.h"

#include <numeric>

#include "ElectoralSystemUtils.h"

int BucklinVoting::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<vector<double>> rankings;
	vector<int> votes(decisionCount, 0);
	rankings.reserve(priorities.size());

	//determine the ranks of all candidates
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	// calculate the number of votes required for a majority
	int majority = rankings.size() / 2 + 1;

	vector<int> winners;
	// this loop needs to start with 1 because it represents the ranks, which start from 1
	// the idea, we start with rank one and sum all the votes. if we have majority the procedure ends
	// if no majority is reached, we add votes for the second preferences, and it is checked whether a condidate has a majority of votes
	// the majority is not updated! it always stays the same. Thus at one point we should have a majority
	for (int i = 1; i <= decisionCount; i++)
	{
		for (int j = 0; j < rankings.size(); j++)
		{
			int winner = ElectoralSystemUtils::determineDecisionWithRank(rankings[j], i);
			votes[winner]++;
		}
		winners = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, remainingCandidates);
		if(winners.size()==1 && votes[winners[0]]>=majority)
		{
			return winners[0];
		}
	}

	return winners[0];
	
}
