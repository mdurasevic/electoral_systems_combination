#include "RankedPairs.h"

#include <algorithm>
#include <numeric>
#include <tuple>

#include "ElectoralSystemUtils.h"

int RankedPairs::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<vector<double>> rankings;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<int> winner(decisionCount, 0);
	rankings.reserve(voterCount);

	// determine the ranks of al candidates
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
	}

	// first determine the tally. the tally denotes for each pair of candidates x and y the number of votes where x was preferred over y
	// the tally is determined only for positive preferences
	auto tally = ElectoralSystemUtils::calculateTallyMatrix(rankings);
	std::vector<std::tuple<int, int, int>> preferences;

	for(int i =0; i<tally.size(); i++)
	{
		for(int j = i+1; j<tally.size(); j++)
		{
			// if i wins over j, then we add it to the tally
			if(tally[i][j]>=0)
			{
				auto tuple = std::make_tuple(i, j, tally[i][j]);
				preferences.push_back(tuple);
			} // otherwise we just reverse the indices, since in that case j received more preferences
			else
			{
				auto tuple = std::make_tuple(j, i, tally[j][i]);
				preferences.push_back(tuple);
			}
		}
	}

	//we sort the list of preferences from higher to lower
	std::sort(preferences.begin(), preferences.end(), [](auto& t1, auto& t2)
		{
			return get<2>(t1) > get<2>(t2);
		});

	// then we determine the lock matrix.
	std::vector<std::vector<int>> lockMatrix(decisionCount, std::vector<int>(decisionCount, 0));

	//we set a 1 if for each pair when x is better than y, and -1 in the opossite case
	for(auto& preference : preferences)
	{
		//however, we only update those values that are not already set!
		// if the current place in the matrix is set we skip it
		if(lockMatrix[get<0>(preference)][get<1>(preference)]==0)
		{
			//set the values
			lockMatrix[get<0>(preference)][get<1>(preference)] = 1;
			lockMatrix[get<1>(preference)][get<0>(preference)] = -1;

			// here we check for transitivity. we update also all those places where the one to which we compared
			// was better than other choices
			for(int i = 0; i<decisionCount; i++)
			{
				if (lockMatrix[get<1>(preference)][i]==1)
				{
					lockMatrix[get<0>(preference)][i] = 1;
					lockMatrix[i][get<0>(preference)] = -1;
				}
			}
		}
	}

	// we now go through the matic and cound the number of locks for each row
	std::vector<int> count(decisionCount, 0);
	for(int i =0; i< lockMatrix.size();i++)
	{
		for(int j=0; j<lockMatrix.size(); j++)
		{
			count[i] += lockMatrix[i][j];
		}
	}

	// the winner is the candidate that has the most locks. it is possible that ties occur. 
	auto winners = ElectoralSystemUtils::getIndicesWithHighestVoteCount(count, remainingCandidates);

	return winners[0];
}
