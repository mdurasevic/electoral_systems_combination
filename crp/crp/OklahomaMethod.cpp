#include "OklahomaMethod.h"
#include "ElectoralSystemUtils.h"

int OklahomaMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<int> winner(decisionCount, 0);

	rankings.reserve(voterCount);
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	// determine the number of choices that can be performed by the voters
	// this depends on the number of candidates
	int numberOfChoices = 1;
	if (priorities[0].size() == 3|| priorities[0].size() == 4)
	{
		numberOfChoices = 2;
	}
	else if (priorities[0].size() > 5)
	{
		numberOfChoices = 3;
	}

	int iteration = 1;
	std::vector<double> votes(priorities[0].size(), 0);

	// we determine the required majority
	int majority = priorities[0].size() / 2 + 1;

	do
	{
		// we consider only the rank of the current iteration (1, 2, or 3)
		for(auto ranks : rankings)
		{
			auto index = ElectoralSystemUtils::determineDecisionWithRank(ranks, iteration);
			// here we scale the votes by the preference position
			votes[index] += 1.0 / iteration;
		}

		//check if we have a single winner
		auto winners = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes,remainingCandidates);
		if (winners.size() == 1 && votes[winners[0]] >= majority)
		{
			return winners[0];
		}
		iteration++;
	} while (iteration <= numberOfChoices);
	
	auto winners = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes,remainingCandidates);
	return winners[0];
}
