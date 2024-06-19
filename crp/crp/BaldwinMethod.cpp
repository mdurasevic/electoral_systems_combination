#include "BaldwinMethod.h"

#include <numeric>

#include "ElectoralSystemUtils.h"

int BaldwinMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);

	// if there is a single candidate, we can return him as winner
	if(decisionCount==1)
	{
		return 0;
	}

	//calculate the ranks for each candidate
	rankings.reserve(voterCount);
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	do {
		vector<int> total(decisionCount, 0.0);

		// determine the scores for all candidate 
		for (const auto& ranking : rankings)
		{
			for (const int candidateIndex : remainingCandidates)
			{
				total[candidateIndex] += decisionCount - (ranking[candidateIndex]);
			}
		}

		// determine the candidates that received the lowest score based on the rankings
		vector<int> looserIndices = ElectoralSystemUtils::getIndicesWithLowestVoteCount(total, remainingCandidates);
		if(looserIndices.size() == remainingCandidates.size())
		{
			return remainingCandidates[0];
		}

		// we need to update the ranks of all those candidates that we will eliminate
		for (const auto looser : looserIndices) {
			for (auto& ranking : rankings)
			{
				// determine the rank of the looser
				const double looserRank = ranking[looser];
				for (const int remainingCandidate : remainingCandidates)
				{
					if (remainingCandidate == looser)
					{
						ranking[remainingCandidate] = decisionCount + 1;
					}
					else if (looserRank < ranking[remainingCandidate])
					{
						ranking[remainingCandidate] -= 1;
					}
				}
			}
		}
		for(int i = looserIndices.size()-1; i>=0; i--)
		{
			remainingCandidates.erase(std::remove(remainingCandidates.begin(), remainingCandidates.end(), looserIndices[i]), remainingCandidates.end());
		}

	} while (remainingCandidates.size() > 1);
		
	return remainingCandidates[0];
}
