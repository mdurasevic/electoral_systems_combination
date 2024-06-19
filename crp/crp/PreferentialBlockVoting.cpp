#include "PreferentialBlockVoting.h"
#include "ElectoralSystemUtils.h"

int PreferentialBlockVoting::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<int> votes(decisionCount, 0);
	vector<vector<double>> rankings;

	// first calculate the ranks for all candidates and also count the first preferences for each of them
	rankings.reserve(voterCount);
	for (auto voter : priorities)
	{
		rankings.push_back(ElectoralSystemUtils::calculateRanksUnique(voter, winnerStrategy));
		const int winner = ElectoralSystemUtils::determineVote(voter, winnerStrategy);
		votes[winner]++;
	}

	int majority = priorities.size() / 2 + 1;

	do
	{
		// we check if there is a single winner (candidate with most votes) and if he has majority. if yes, we proclaim him the winner
		auto winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes,remainingCandidates);
		if(winnerIndices.size() == 1 && votes[winnerIndices[0]]>=majority)
		{
			return winnerIndices[0];
		}

		// we determine the loosers, the candidates that received the lowest number of first prefeences
		auto loosers = ElectoralSystemUtils::getIndicesWithLowestVoteCount(votes,remainingCandidates);

		//in case the number of loosers is equal to the number of remaining candidates, we have a tie that we cannot resolve. 
		if(loosers.size() == remainingCandidates.size())
		{
			return winnerIndices[0];
		}

		// we then iterate through all loosers to update the ranks
		for (int looser : loosers)
		{

			for (auto& ranking : rankings)
			{
				// first, we determine the rank of the looser that we eliminate
				int looserRank = ranking[looser];
				for (int remainingCandidate : remainingCandidates)
				{
					//is tis required????
					if (remainingCandidate == looser)
					{
						ranking[remainingCandidate] = decisionCount + 1;
					}
					// if the looser rank is lower than the considered rank, we dectement it.
					// basically we just shift all the ranks larger than the rank of the looser. 
					else if (looserRank < ranking[remainingCandidate])
					{
						ranking[remainingCandidate] -= 1;
					}
				}
			}
			// and finally we eliminate the looser from the remaining candidates. 
			remainingCandidates.erase(std::remove(remainingCandidates.begin(), remainingCandidates.end(), loosers[0]), remainingCandidates.end());
		}


		for (auto ranking : rankings)
		{
			int winner = ElectoralSystemUtils::determineVoteFromSubset(ranking, winnerStrategy, remainingCandidates);
			votes[winner]++;
		}

	} while (true);

}
