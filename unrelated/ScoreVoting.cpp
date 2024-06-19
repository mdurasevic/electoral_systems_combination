#include "ScoreVoting.h"

#include <numeric>

#include "ElectoralSystemUtils.h"

ScoreVoting::ScoreVoting(int maxScore, int minScore)
{
	this->maxScore = maxScore;
	this->minScore = minScore;
}

int ScoreVoting::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	std::vector<int> totalScores(priorities[0].size(), 0);
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);

	// each voter scores all the candidates
	for(auto& voter : priorities)
	{
		//calculate the score. we divide the interval of priorites (determined by the minimum and maximum values obtainer) into a fixed number of subintervals
		// and based on which subinterval a certain priority value belongs to, we assign a corresponding score 
		auto decisionScores = ElectoralSystemUtils::calculateScoresUniform(voter, winnerStrategy, maxScore, minScore);

		// we add the scores to each candidate
		for(int i = 0; i<decisionScores.size(); i++)
		{
			totalScores[i] += decisionScores[i];
		}
	}


	// we determine the winners
	vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(totalScores, remainingCandidates);
	if(winnerIndices.size()==1)
	{
		return winnerIndices[0];
	}


	// if we have a tie, use plorality to see whom to select
	vector<int> votes(priorities[0].size(), 0);

	// first determine the rankings
	for (auto voter : priorities)
	{
		//here we immediately determine the votes in case there is a tie after the borda count
		int winner = ElectoralSystemUtils::determineVoteFromSubset(voter, winnerStrategy, winnerIndices);
		votes[winner]++;
	}
		
	auto newWinnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, winnerIndices);
	return newWinnerIndices[0];
}
