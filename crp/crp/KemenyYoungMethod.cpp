#include "KemenyYoungMethod.h"

#include <algorithm>
#include <iostream>
#include <numeric>

#include "ElectoralSystemUtils.h"

KemenyYoungMethod::KemenyYoungMethod(int maximumConsideredCandidates)
{
	this->maximumConsideredCandidates = maximumConsideredCandidates;
}

int KemenyYoungMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);

	// determine the rankings of all candidates
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	//calculate the sum matrix of all pairs candidates
	auto sumMatrix = ElectoralSystemUtils::calculateCondorcetMatrix(rankings);

	// determine whether there is a condorcet winner
	const int condorcetWinnerIndex = ElectoralSystemUtils::determineCondorcetWinner(sumMatrix);
	if (condorcetWinnerIndex != -1)
	{
		return condorcetWinnerIndex;
	}
	
	vector<int> order(decisionCount);
	std::iota(order.begin(), order.end(), 0);
	int bestScore = 0;
	std::vector<int> winner;

	//the problem of this method that it is NP complete, i.e. all permutations need to be calculated
	// for more than 10 candidates this becomes quite expensive, therefore in case if there are more
	// than a given number of candidates we first eliminate a set of candidates and then proceed
	// to the Kemeny young method
	while(order.size()>maximumConsideredCandidates)
	{
		vector<int> total(decisionCount, 0.0);

		// determine the scores for all candidate 
		for (auto& ranking : rankings)
		{
			for (const int candidateIndex : order)
			{
				total[candidateIndex] += decisionCount - (ranking[candidateIndex]);
			}
		}

		// determine the candidates that received the lowest score based on the rankings
		vector<int> looserIndices = ElectoralSystemUtils::getIndicesWithLowestVoteCount(total, order);
		if (looserIndices.size() == order.size())
		{
			return order[0];
		}

		// we need to update the ranks of all those candidates that we will eliminate
		for (const auto looser : looserIndices) {
			for (auto& ranking : rankings)
			{
				// determine the rank of the looser
				const int looserRank = ranking[looser];
				for (int j = 0; j < order.size(); j++)
				{
					int currentOrder = order[j];
					if (currentOrder == looser)
					{
						ranking[currentOrder] = decisionCount + 1;
					}
					else if (looserRank < ranking[currentOrder])
					{
						ranking[currentOrder] -= 1;
					}
				}
			}
		}
		for (int i = looserIndices.size() - 1; i >= 0; i--)
		{
			order.erase(std::remove(order.begin(), order.end(), looserIndices[i]), order.end());
		}
	}

	do
	{
		const int score = ElectoralSystemUtils::calculatePermutationScore(order, sumMatrix);
		if(score>bestScore)
		{
			winner.clear();
			winner.push_back(order[0]);
		} else if(score == bestScore)
		{
			winner.push_back(order[0]);
		}
	} while (std::next_permutation(order.begin(), order.end()));

	//in case of ties, we just select the first permutation
	return winner[0];
}
