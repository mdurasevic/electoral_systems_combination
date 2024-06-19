#include "JudgementMethod.h"

#include <numeric>

#include "ElectoralSystemUtils.h"

JudgementMethod::JudgementMethod(int maximumRating, int minimumRating, MedianRule medianRule)
{
	this->minimumRating = minimumRating;
	this->maximumRating = maximumRating;
	this->medianRule = medianRule;
}

int JudgementMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	std::vector<vector<int>> totalScores;
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	const int voterCount = ElectoralSystemUtils::getNumberOfVoters(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);

	// calculate the scores each voter gives to each candidate
	for (int i = 0; i < priorities.size(); i++)
	{
		const auto decisionScores = ElectoralSystemUtils::calculateScoresUniform(priorities[i], winnerStrategy, maximumRating, minimumRating);
		totalScores.push_back(decisionScores);
	}

	//we now reorder the scores that we get a list of scores for each candidate
	std::vector<std::vector<int>> scoresPerCandidate(decisionCount, std::vector<int>(voterCount,0));

	for (int i = 0; i < priorities[0].size(); i++)
	{
		for (int j = 0; j < priorities.size(); j++)
		{
			scoresPerCandidate[i][j] = totalScores[j][i];
		}
	}

	// we determine the median score for each candidate
	std::vector<int> medians;
	medians.reserve(scoresPerCandidate.size());
	for (auto scores_per_candidate : scoresPerCandidate)
	{
		medians.push_back(ElectoralSystemUtils::calculateMedian(scores_per_candidate));
	}

	// if there is only one candidate that obtained the highest median score, we select him
	auto winners = ElectoralSystemUtils::getIndicesWithHighestVoteCount(medians, remainingCandidates);
	if (winners.size() == 1)
	{
		return winners[0];
	}

	// if not, we use a median strategy to decide the winner 
	vector<double> numberOfProponents(scoresPerCandidate.size(), 0);
	vector<double> numberOfOpponents(scoresPerCandidate.size(), 0);

	//calculate the number of proponents and opponents, i.e. number of voters that
	//gave a score larger than the median, and a score lower than the median, respectively
	for (int i = 0; i < scoresPerCandidate.size(); i++)
	{
		int median = medians[i];
		for (int j=0; j < scoresPerCandidate[i].size(); j++)
		{
			int candidateScore = scoresPerCandidate[i][j];
			if (candidateScore< median)
			{
				numberOfOpponents[i]++;
			}
			else if (candidateScore> median)
			{
				numberOfProponents[i]++;
			}
		}
	}

	//now we just normalise this with the number of voters
	int scoreCount = scoresPerCandidate[0].size();
	for(int i =0; i<numberOfProponents.size(); i++)
	{
		numberOfOpponents[i] /= scoreCount;
		numberOfProponents[i] /= scoreCount;
	}


	vector<double> scores(scoresPerCandidate.size(),0);
	// we use a certain median rule for tie breaking
	// in this case we look at the highest difference between the proponents and opponents
	if (medianRule == MedianRule::TypicalJudgement)
	{
		for(int i = 0; i<scoresPerCandidate.size(); i++)
		{
			scores[i] = medians[i] + numberOfProponents[i] - numberOfOpponents[i];
		}
	}
	else if (medianRule == MedianRule::UsualJudgement)
	{
		for (int i = 0; i < scoresPerCandidate.size(); i++)
		{
			scores[i] = medians[i] + 0.5 * (numberOfProponents[i] - numberOfOpponents[i])/(1.0-numberOfProponents[i] - numberOfOpponents[i]);
		}
	} else if(medianRule == MedianRule::CentralJudgement)
	{
		for (int i = 0; i < scoresPerCandidate.size(); i++)
		{
			//we need to add a small epsilon to ensure that we do not have division by zero, otherwise the method will not work. 
			scores[i] = medians[i] + 0.5 * (numberOfProponents[i] - numberOfOpponents[i]) / (numberOfProponents[i] + numberOfOpponents[i]+10e-6);
		}
	}
	else if (medianRule == MedianRule::MajorityJudgement)
	{
		for (int i = 0; i < scoresPerCandidate.size(); i++)
		{
			scores[i] = medians[i] + (numberOfProponents[i]>numberOfOpponents[i]? numberOfProponents[i] : -numberOfOpponents[i]);
		}
	}

	// the one with the highest score is the winner, in case of ties we select the one with the lowest index
	auto highestMedianIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(scores, remainingCandidates);
	return highestMedianIndices[0];
}
