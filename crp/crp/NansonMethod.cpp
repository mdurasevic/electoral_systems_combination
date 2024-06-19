#include "NansonMethod.h"

#include <numeric>

#include "ElectoralSystemUtils.h"

int NansonMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);

	// if there is a single candidate, we can return him as winner
	if (decisionCount == 1)
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
		for (auto& ranking : rankings)
		{
			for (int candidateIndex : remainingCandidates)
			{
				total[candidateIndex] += decisionCount - (ranking[candidateIndex]);
			}
		}

		// calculate the average score
		double totalScore = 0.0;
		const double numberOfScores = remainingCandidates.size();
		for (const int remainingCandidate : remainingCandidates)
		{
			totalScore += total[remainingCandidate];
		}
		const double averageScore = totalScore / numberOfScores;


		// determine the candidates that received the scores below the average borda score
		vector<int> looserIndices = ElectoralSystemUtils::getIndicesBelowAverage(total, remainingCandidates, averageScore);
		if (looserIndices.size() == 0 || looserIndices.size() == remainingCandidates.size())
		{
			return remainingCandidates[0];
		}

		// we need to update the ranks of all those candidates that we will eliminate
		for (auto looser : looserIndices) {
			for (int i = 0; i < rankings.size(); i++)
			{
				// determine the rank of the looser
				const int looserRank = rankings[i][looser];
				for (int remainingCandidate : remainingCandidates)
				{
					// 
					if (remainingCandidate == looser)
					{
						rankings[i][remainingCandidate] = decisionCount + 1;
					}
					else if (looserRank < rankings[i][remainingCandidate])
					{
						rankings[i][remainingCandidate] -= 1;
					}
				}
			}
		}
		for (int i = looserIndices.size() - 1; i >= 0; i--)
		{
			remainingCandidates.erase(std::remove(remainingCandidates.begin(), remainingCandidates.end(), looserIndices[i]), remainingCandidates.end());
		}

	} while (remainingCandidates.size() > 1);

	return remainingCandidates[0];

}
