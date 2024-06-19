#include "TidemanAlternativeSmith.h"
#include <numeric>
#include "ElectoralSystemUtils.h"

int TidemanAlternativeSmith::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{

	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	
	vector<vector<double>> rankings;
	rankings.reserve(voterCount);
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	auto preferenceMatrix = ElectoralSystemUtils::calculateSmithPreferencetMatrix(rankings);
	do {
		auto copelandScore = ElectoralSystemUtils::calculateCopelandScoreFromPreferenceMatrix(preferenceMatrix, remainingCandidates);
		auto sortedIndices = ElectoralSystemUtils::sortByScores(remainingCandidates, copelandScore, highest);
		auto copelandWinners = ElectoralSystemUtils::getIndicesWithHighestVoteCount(copelandScore, remainingCandidates);
		int smithSetCardinality;
		vector<int> smithSet;
		smithSet.reserve(copelandWinners.size());

		for (int i = 0; i < copelandWinners.size(); i++)
		{
			smithSet.push_back(sortedIndices[i]);
		}

		do
		{
			smithSetCardinality = smithSet.size();
			for (int j = smithSetCardinality; j < remainingCandidates.size(); j++)
			{
				for (int i = 0; i < smithSetCardinality; i++)
				{
					int index = smithSet[i];
					int comparison = sortedIndices[j];
					if (preferenceMatrix[comparison][index] != 0)
					{
						smithSet.push_back(comparison);
						break;
					}
				}

			}
		} while (smithSet.size() > smithSetCardinality);

		if (smithSet.size() == 1)
		{
			return smithSet[0];
		}

		vector<int> votes(decisionCount, 0);

		// determine the votes for the candidates where the candidate was ranked the lowest
		for (auto& voter : rankings)
		{
			const int winner = ElectoralSystemUtils::determineVoteFromSubset(voter, highest, smithSet);
			votes[winner]++;
		}
		vector<int> looserIndices = ElectoralSystemUtils::getIndicesWithLowestVoteCount(votes, smithSet);

		remainingCandidates = smithSet;

		if(looserIndices.size() == smithSet.size())
		{
			return smithSet[0];
		}
		//std::cout << smithSet.size()<< " " << looserIndices.size()<<endl;
		// remove loosers from the remaining candidate list
		for (int looserIndex : looserIndices)
		{
			remainingCandidates.erase(std::remove(remainingCandidates.begin(), remainingCandidates.end(), looserIndex), remainingCandidates.end());
		}

		
	} while (true);

}
