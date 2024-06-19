#include "ContigentVoting.h"

#include <numeric>

#include "ElectoralSystemUtils.h"
#include "LowestPriorityStrategy.h"

int ContigentVoting::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<int> votes(decisionCount, 0);
	rankings.reserve(voterCount);

	shared_ptr<LowestPriorityStrategy> lowestPriorityStrategy = make_shared<LowestPriorityStrategy>();

	// first determine the rankings
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	// we calculate the votes based on the first preferences
	for (int i = 0; i < rankings.size(); i++)
	{
		const int winner = ElectoralSystemUtils::determineVote(rankings[i], lowestPriorityStrategy);
		votes[winner]++;
	}

	// we now check if we have a single winner and he has the majority (it should be enough to check only the majority...)
	auto winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, remainingCandidates);
	//int majority = priorities.size() / 2 + 1;
	// if we have he is the winner
	if(winnerIndices.size()==1 && ElectoralSystemUtils::hasMajority(votes, winnerIndices[0], priorities.size()))
	{
		return winnerIndices[0];
	}

	// if there was only a single winner in the first round, we have to find the second one as well and add him for consideration
	if(winnerIndices.size()==1)
	{
		const auto secondBestIndices = ElectoralSystemUtils::determineSecondBestCandidates(votes, winnerIndices[0]);
		for (const int second_best_index : secondBestIndices)
		{
			winnerIndices.push_back(second_best_index);
		}
	}


	// if we don't we reset the votes, and consider only the two best candidates
	std::fill(votes.begin(), votes.end(), 0);

	for (int i = 0; i < rankings.size(); i++)
	{
		int winner = ElectoralSystemUtils::determineVoteFromSubset(rankings[i], lowestPriorityStrategy, winnerIndices);
		votes[winner]++;
	}


	return winnerIndices[0];

}
