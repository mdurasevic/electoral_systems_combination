#include "BordaCount.h"

#include <iostream>
#include <numeric>

#include "ElectoralSystemUtils.h"

BordaCount::BordaCount(BordaCountStrategy gradingStrategy)
{
	this->gradingStrategy = gradingStrategy;
}

int BordaCount::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<int> votes(decisionCount, 0);
	vector<vector<double>> rankings;
	rankings.reserve(voterCount);

	// first determine the rankings
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
		//here we immediately determine the votes in case there is a tie after the borda count
		const int winner = ElectoralSystemUtils::determineVote(voter, winnerStrategy);
		votes[winner]++;
	}

	vector<double> total(decisionCount, 0.0);

	// we sum all the rankings based on the rankings
	for (auto& ranking : rankings)
	{
		for(int j = 0; j< ranking.size(); j++)
		{
			if(gradingStrategy == BordaCountStrategy::StandardBorda)
			{
				// standard borda strategy, the lower the ranking, the higher the points n-rank
				total[j] += decisionCount - (ranking[j]);
			}
			else if(gradingStrategy == BordaCountStrategy::Nauru)
			{
				// we divide 1 by the rank
				total[j] += 1.0/(ranking[j]);
			} else if(gradingStrategy == BordaCountStrategy::BinaryNumberSystem)
			{
				total[j] += 1.0/std::pow(2, ranking[j]);
			} else if(gradingStrategy == BordaCountStrategy::Eurovision)
			{
				if(ranking[j]<=eurovisionScores.size())
				{
					total[j] += eurovisionScores[ranking[j] - 1];
				}
			}
		}
	}

	// determine the winner
	vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(total,remainingCandidates);

	// if there is more than one winner, try with plurality
	if(winnerIndices.size()>1)
	{
		auto newWinnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, winnerIndices);
		return newWinnerIndices[0];
	}

	return winnerIndices[0];
}
