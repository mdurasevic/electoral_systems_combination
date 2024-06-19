#include "ElectoralSystemUtils.h"

#include <algorithm>
#include <numeric>

int ElectoralSystemUtils::determineVote(std::vector<double> &voter, std::shared_ptr<WinnerStrategy> winnerStrategy)
{
	auto min_it = std::min_element(voter.begin(), voter.end());
	auto max_it = std::max_element(voter.begin(), voter.end());

	int min_index = std::distance(voter.begin(), min_it);
	int max_index = std::distance(voter.begin(), max_it);
	int winnerIndex = winnerStrategy->winner(min_index, max_index);
	return winnerIndex;
}

int ElectoralSystemUtils::determineVoteFromSubset(std::vector<double>& voter, std::shared_ptr<WinnerStrategy> winnerStrategy, std::vector<int>& candidates)
{
	const int initialCandidate = candidates[0];
	int min_index = initialCandidate;
	int max_index = initialCandidate;

	const double initialCandidatePriority = voter[initialCandidate];
	double minValue = initialCandidatePriority;
	double maxValue = initialCandidatePriority;

	for (const int candidate : candidates)
	{
		double priority = voter[candidate];
		if (priority > maxValue)
		{
			maxValue = priority;
			max_index = candidate;
		}

		if (priority < minValue)
		{
			minValue = priority;
			min_index = candidate;
		}
	}
	const int winnerIndex = winnerStrategy->winner(min_index, max_index);
	return winnerIndex;
}

template<typename T>
std::vector<int> ElectoralSystemUtils::getIndicesWithHighestVoteCount(std::vector<T>& votes, std::vector<int>& candidateIndices)
{
	T max_votes = votes[candidateIndices[0]];

	for (int i = 1; i < candidateIndices.size(); i++)
	{
		const T candidateVotes = votes[candidateIndices[i]];
		if (max_votes < candidateVotes)
		{
			max_votes = candidateVotes;
		}
	}
	std::vector<int> winnerIndices;

	for (const int& candidateIndice : candidateIndices)
	{
		if (votes[candidateIndice] == max_votes)
		{
			winnerIndices.push_back(candidateIndice);
		}
	}

	return winnerIndices;
}

template std::vector<int> ElectoralSystemUtils::getIndicesWithHighestVoteCount(std::vector<int>&,std::vector<int>&);
template std::vector<int> ElectoralSystemUtils::getIndicesWithHighestVoteCount(std::vector<double>&,std::vector<int>&);

bool ElectoralSystemUtils::hasMajority(std::vector<int>& votes, int candidate, int voterCount)
{
	return (voterCount / 2) < votes[candidate];
}

std::vector<int> ElectoralSystemUtils::determineSecondBestCandidates(std::vector<int>& votes, int winnerIndex)
{
	std::vector<int> secondMaxVoteCandidates;
	int secondBestVoteCount = 0;
	std::vector<int> secondBestIndices;

	for(int i=0; i<votes.size(); i++)
	{
		const int voteCount = votes[i];
		if(winnerIndex!=i && voteCount>secondBestVoteCount)
		{
			secondBestVoteCount = voteCount;
			secondBestIndices.clear();
			secondBestIndices.push_back(i);
		} else if(voteCount == secondBestVoteCount)
		{
			secondBestIndices.push_back(i);
		}
	}
	return secondBestIndices;
}


template<typename T>
std::vector<int> ElectoralSystemUtils::getIndicesWithLowestVoteCount(std::vector<T>& votes,
	std::vector<int>& candidateIndices)
{
	T min_votes = votes[candidateIndices[0]];

	for (int i = 1; i < candidateIndices.size(); i++)
	{
		const int candidateVotes = votes[candidateIndices[i]];
		if (min_votes> candidateVotes)
		{
			min_votes = candidateVotes;
		}
	}

	std::vector<int> winnerIndices;
	for (const int& candidateIndice : candidateIndices)
	{
		if (votes[candidateIndice] == min_votes)
		{
			winnerIndices.push_back(candidateIndice);
		}
	}

	return winnerIndices;
}

template std::vector<int> ElectoralSystemUtils::getIndicesWithLowestVoteCount(std::vector<int>&, std::vector<int>&);
template std::vector<int> ElectoralSystemUtils::getIndicesWithLowestVoteCount(std::vector<double>&, std::vector<int>&);

std::vector<int> ElectoralSystemUtils::getIndicesBelowAverage(std::vector<int>& scores,
	std::vector<int>& candidateIndices, double averageScore)
{ 
	std::vector<int> looserIndices;

	for (const int candidateIndice : candidateIndices)
	{
		if(scores[candidateIndice]<averageScore)
		{
			looserIndices.push_back(candidateIndice);
		}
	}

	return looserIndices;
}

std::vector<double> ElectoralSystemUtils::calculateRanksFractional(std::vector<double>& priorities, std::shared_ptr<WinnerStrategy> winnerStrategy)
{
	int n = priorities.size();
	std::vector<double> R(n, 0);
	std::vector <std::pair<double, int>> T(n);

	for (int i = 0; i < n; i++)
	{
		T[i].first = priorities[i];
		T[i].second = i;
	}

	sort(T.begin(), T.end(), [&](std::pair<double, int> a, std::pair<double, int> b) -> bool {return winnerStrategy->leftIsBetter(a.first, b.first); });
	float rank = 1, m = 1, i = 0;

	while (i < n) {
		float j = i;

		while (j < n - 1 && T[j].first == T[j + 1].first) {
			j += 1;
		}

		m = j - i + 1;

		for (int k = 0; k < m; k++) {
			int idx = T[i + k].second;
			R[idx] = rank + (m - 1) * 0.5;
		}

		rank += m;
		i += m;
	}
	return R;
}

std::vector<double> ElectoralSystemUtils::calculateRanksUnique(std::vector<double>& priorities,
	std::shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int n = priorities.size();
	std::vector<double> R(n, 0);
	std::vector <std::pair<double, int>> T(n);

	for (int i = 0; i < n; i++)
	{
		T[i].first = priorities[i];
		T[i].second = i;
	}

	sort(T.begin(), T.end(), [&](std::pair<double, int> a, std::pair<double, int> b) -> bool {return winnerStrategy->leftIsBetter(a.first, b.first); });
	int rank = 1, i = 0;

	while (i < n) {
		const int idx = T[i].second;
		R[idx] = rank;
		rank += 1;
		i += 1;
	}
	return R;
}

std::vector<std::vector<int>> ElectoralSystemUtils::calculateCondorcetMatrix(std::vector<std::vector<double>>& ranks)
{
	const int numberOfCandidates = getNumberOfDecisions(ranks);
	std::vector<std::vector<int>> condorcetMatrix(numberOfCandidates, std::vector<int>(numberOfCandidates, 0));
	
	for (auto voter : ranks)
	{
		for(int i=0;i<voter.size(); i++)
		{
			for(int j=i+1; j<voter.size();j++)
			{
				if(voter[i]<voter[j])
				{
					condorcetMatrix[i][j] += 1;
				} else
				{
					condorcetMatrix[j][i] += 1;
				}
			}
		}
	}

	return condorcetMatrix;
}

int ElectoralSystemUtils::determineCondorcetWinner(std::vector<std::vector<int>>& sumMatrix)
{
	for (int i = 0; i < sumMatrix.size(); i++)
	{
		bool isCondorcetWinner = true;
		for (int j = 0; j < sumMatrix.size(); j++)
		{
			if (i != j && sumMatrix[i][j] < sumMatrix[j][i])
			{
				isCondorcetWinner = false;
			}
		}
		if (isCondorcetWinner)
		{
			return i;
		}
	}
	return -1;
}

std::vector<std::vector<int>> ElectoralSystemUtils::calculateStrongestPaths(std::vector<std::vector<int>>& sumMatrix)
{ 
	const int numberOfCandidates = sumMatrix[0].size();
	std::vector<std::vector<int>> strengthMatrix(numberOfCandidates, std::vector<int>(numberOfCandidates,0));

	for(int i =0; i<numberOfCandidates; i++)
	{
		for(int j =0; j<numberOfCandidates; j++)
		{
			if(i!=j)
			{
				if(sumMatrix[i][j]>sumMatrix[j][i])
				{
					strengthMatrix[i][j] = sumMatrix[i][j];
				}
			}
		}
	}


	for (int i = 0; i < numberOfCandidates; i++)
	{
		for (int j = 0; j < numberOfCandidates; j++)
		{
			if (i != j)
			{
				for(int k=0; k<numberOfCandidates;k++)
				{
					if(i!=k && j!=k)
					{
						strengthMatrix[j][k] = (int)std::max(strengthMatrix[j][k], std::min(strengthMatrix[j][i], strengthMatrix[i][k]));
					}
				}
			}
		}
	}

	return strengthMatrix;
}

std::vector<std::vector<int>> ElectoralSystemUtils::calculateTallyMatrix(std::vector<std::vector<double>>& ranks)
{
	int numberOfCandidates = ranks[0].size();
	std::vector<std::vector<int>> condorcetMatrix(numberOfCandidates, std::vector<int>(numberOfCandidates, 0));

	for (auto voter : ranks)
	{
		for (int i = 0; i < voter.size(); i++)
		{
			for (int j = i + 1; j < voter.size(); j++)
			{
				if (voter[i] < voter[j])
				{
					condorcetMatrix[i][j] += 1;
					condorcetMatrix[j][i] -= 1;
				}else
				{
					condorcetMatrix[i][j] -= 1;
					condorcetMatrix[j][i] += 1;
				}
			}
		}
	}

	return condorcetMatrix;
}


std::vector<int> ElectoralSystemUtils::calculateCopelandScore(std::vector<std::vector<double>>& ranks)
{
	
	int numberOfCandidates = ranks[0].size();
	std::vector<int> wins(numberOfCandidates, 0);

	for (auto voter : ranks)
	{
		for (int i = 0; i < voter.size(); i++)
		{
			for (int j = i + 1; j < voter.size(); j++)
			{
				if (voter[i] < voter[j])
				{
					wins[i] += 1;
					wins[j] -= 1;
				}
				else
				{
					wins[j] += 1;
					wins[i] -= 1;
				}
			}
		}
	}

	return wins;
}

std::vector<double> ElectoralSystemUtils::calculateCopelandScoreFromPreferenceMatrix(std::vector<std::vector<double>>& preferences, std::vector<int>& candidates)
{

	int numberOfCandidates = preferences[0].size();
	std::vector<double> wins(numberOfCandidates, 0);

	for (int i = 0; i < candidates.size(); i++)
	{
		for (int j = 0; j < candidates.size(); j++)
		{
			wins[i] += preferences[candidates[i]][candidates[j]];
		}
	}


	return wins;
}

int ElectoralSystemUtils::calculatePermutationScore(std::vector<int>& order, std::vector<std::vector<int>>& preferences)
{
	int score = 0;
	for(int i =0; i<order.size(); i++)
	{
		for(int j=i+1; j<order.size(); j++)
		{
			score += preferences[order[i]][order[j]];
		}
	}
	return score;
}

std::vector<int> ElectoralSystemUtils::calculatePairwiseDefeats(std::vector<std::vector<int>>& sumMatrix)
{
	std::vector<int> defeats(sumMatrix.size(), 0);
	for (int i = 0; i<sumMatrix.size(); i++)
	{
		for(int j = 0; j<sumMatrix.size(); j++)
		{
			if(sumMatrix[i][j]<sumMatrix[j][i] && defeats[i]<sumMatrix[j][i])
			{
				defeats[i] = sumMatrix[j][i];
			}
		}
	}
	return defeats;
}

std::vector<int> ElectoralSystemUtils::calculatePairwiseDefeatMargins(std::vector<std::vector<int>>& sumMatrix)
{
	std::vector<int> defeats(sumMatrix.size(), std::numeric_limits<int>::min());
	for (int i = 0; i < sumMatrix.size(); i++)
	{
		for (int j = 0; j < sumMatrix.size(); j++)
		{
			if(i==j)
			{
				continue;
			}
			int defeat = sumMatrix[j][i] - sumMatrix[i][j];
			if (defeat > defeats[i])
			{
				defeats[i] = defeat;
			}
		}
	}
	return defeats;
}

std::vector<int> ElectoralSystemUtils::calculatePairwiseOpposition(std::vector<std::vector<int>>& sumMatrix)
{

	std::vector<int> defeats(sumMatrix.size(), 0);
	for (int i = 0; i < sumMatrix.size(); i++)
	{
		for (auto& j : sumMatrix)
		{
			if (defeats[i] < j[i])
			{
				defeats[i] = j[i];
			}
		}
	}
	return defeats;
}

int ElectoralSystemUtils::determineDecisionWithRank(std::vector<double>& rankings, int rank)
{
	for(int i = 0; i<rankings.size(); i++)
	{
		if(rankings[i] == rank)
		{
			return i;
		}
	}
	return 0;
}

std::vector<int> ElectoralSystemUtils::calculateScoresUniform(std::vector<double>& voter,
	std::shared_ptr<WinnerStrategy> winnerStrategy, int maximumScore, int minimumScore)
{
	std::vector<int> scores;
	auto min_it = std::min_element(voter.begin(), voter.end());
	auto max_it = std::max_element(voter.begin(), voter.end());
	double maxPriority = *max_it;
	double minPriority = *min_it;

	double worstValue;
	int better = winnerStrategy->winner(0, 1);
	if(better == 0)
	{
		worstValue = maxPriority;
	} else
	{
		worstValue = minPriority;
	}

	for (double priority : voter)
	{
		const double denominator = (maxPriority - minPriority);
		int score = round(minimumScore + (maximumScore - minimumScore) * (priority - worstValue) / denominator);


		if(denominator<10e-6)
		{
			score = maximumScore;
		}
		
		if(score<0)
		{
			score *= -1;
		}
		scores.push_back(score);
	}

	return scores;
}


std::vector<double> ElectoralSystemUtils::calculateScoresUniform(std::vector<double>& voter,
	std::shared_ptr<WinnerStrategy> winnerStrategy, int maximumScore, int minimumScore, std::vector<int> candidateIndices)
{
	std::vector<double> scores;

	double max_votes = voter[candidateIndices[0]];
	double min_votes = voter[candidateIndices[0]];


	for (int i = 1; i < candidateIndices.size(); i++)
	{
		if (max_votes < voter[candidateIndices[i]])
		{
			max_votes = voter[candidateIndices[i]];
		}
		if (min_votes > voter[candidateIndices[i]])
		{
			min_votes = voter[candidateIndices[i]];
		}
	}

	double worstValue;
	int better = winnerStrategy->winner(0, 1);
	if (better == 0)
	{
		worstValue = max_votes;
	}
	else
	{
		worstValue = min_votes;
	}

	for (double priority : voter)
	{
		double denominator = (max_votes - min_votes);
		int score = round(minimumScore + (maximumScore - minimumScore) * (priority - worstValue) / denominator);


		if (denominator < 10e-6)
		{
			score = maximumScore;
		}

		if (score < 0)
		{
			score *= -1;
		}
		scores.push_back(score);
	}

	return scores;
}

std::vector<double> ElectoralSystemUtils::normaliseScores(std::vector<double>& voter,
	std::shared_ptr<WinnerStrategy> winnerStrategy, int maximumScore, int minimumScore, std::vector<int> candidateIndices)
{
	std::vector<double> scores;
	const double initialCandidatePriority = voter[candidateIndices[0]];

	double max_votes = initialCandidatePriority;
	double min_votes = initialCandidatePriority;
	double total_sum = initialCandidatePriority;
	for (int i = 1; i < candidateIndices.size(); i++)
	{
		const double candidatePriority = voter[candidateIndices[i]];
		if (max_votes < candidatePriority)
		{
			max_votes = candidatePriority;
		}
		if (min_votes > candidatePriority)
		{
			min_votes = candidatePriority;
		}
		total_sum += std::abs(candidatePriority);
	}

	std::vector<double> normalisedScores(voter.size(),0);

	if (winnerStrategy->leftIsBetter(0, 1))
	{
		total_sum -= max_votes * candidateIndices.size();
		total_sum *= -1;
	}
	else
	{
		total_sum -= min_votes * candidateIndices.size();
	}

	double ratio = (maximumScore - minimumScore) / total_sum;

	for (const int candidateIndice : candidateIndices)
	{
		if(winnerStrategy->leftIsBetter(0,1))
		{
			normalisedScores[candidateIndice] = (max_votes - voter[candidateIndice]) * ratio;
		} else
		{
			normalisedScores[candidateIndice] = (voter[candidateIndice] - min_votes) * ratio;
		}
	}

	return normalisedScores;
}


std::vector<int> ElectoralSystemUtils::determineApprovedCandidates(std::vector<double>& priorities,
	std::shared_ptr<WinnerStrategy> winnerStrategy, double approvalRate){
	std::vector<int> approvedCandidates(priorities.size(),0);
	const int n = priorities.size();
	std::vector<double> R(n, 0);
	std::vector <std::pair<double, int>> T(n);

	for (int i = 0; i < n; i++)
	{
		T[i].first = priorities[i];
		T[i].second = i;
	}

	sort(T.begin(), T.end(), [&](std::pair<double, int> a, std::pair<double, int> b) -> bool {return winnerStrategy->leftIsBetter(a.first, b.first); });
	int numberOfApproved = static_cast<int>(approvalRate * priorities.size());
	numberOfApproved = numberOfApproved == 0 ? 1 : numberOfApproved;

	for(int i = 0; i<numberOfApproved; i++)
	{
		approvedCandidates[T[i].second]=1;
	}

	return approvedCandidates;
}

std::vector<int> ElectoralSystemUtils::determineApprovedCandidatesByInterval(std::vector<double>& priorities,
	std::shared_ptr<WinnerStrategy> winnerStrategy, double approvalRate) {
	std::vector<int> approvedCandidates(priorities.size(), 0);

	auto min_it = std::min_element(priorities.begin(), priorities.end());
	auto max_it = std::max_element(priorities.begin(), priorities.end());

	double bestPriority = winnerStrategy->winner(0,1);

	double interval = ( *max_it - *min_it)*approvalRate;
	if(bestPriority == 0)
	{
		const double upperBound = *min_it + interval;
		for(int i=0;i<priorities.size(); i++)
		{
			if(priorities[i]<=upperBound)
			{
				approvedCandidates[i] = 1;
			}
		}
	} else
	{
		const double lowerBound = *max_it - interval;
		for (int i = 0; i < priorities.size(); i++)
		{
			if (priorities[i] >= lowerBound)
			{
				approvedCandidates[i] = 1;
			}
		}
	}
	
	return approvedCandidates;
}


std::vector<std::vector<int>> ElectoralSystemUtils::calculatePreferencetMatrix(std::vector<std::vector<double>>& ranks)
{
	std::vector<std::vector<int>> concordateMatrix;
	int numberOfCandidates = ranks[0].size();

	for (int i = 0; i < numberOfCandidates; i++)
	{
		concordateMatrix.push_back(std::vector<int>(numberOfCandidates, 0));
	}

	for (auto voter : ranks)
	{
		for (int i = 0; i < voter.size(); i++)
		{
			for (int j = i + 1; j < voter.size(); j++)
			{
				if (voter[i] > voter[j])
				{
					concordateMatrix[i][j] += 1;
				}
				else
				{
					concordateMatrix[j][i] += 1;
				}
			}
		}
	}

	return concordateMatrix;
}


int ElectoralSystemUtils::calculateMedian(std::vector<int>& scores)
{
	size_t n = scores.size() / 2;
	std::nth_element(scores.begin(), scores.begin() + n, scores.end());
	return scores[n];
}

int ElectoralSystemUtils::getNumberOfDecisions(std::vector<std::vector<double>> priorities)
{
	return priorities[0].size();
}

int ElectoralSystemUtils::getNumberOfVoters(std::vector<std::vector<double>> priorities)
{
	return priorities.size();
}

std::vector<int> ElectoralSystemUtils::createInitialCandidateList(int numberOfCandidates)
{
	std::vector<int> remainingCandidates(numberOfCandidates);
	std::iota(remainingCandidates.begin(), remainingCandidates.end(), 0);
	return remainingCandidates;
}

std::vector<std::vector<double>> ElectoralSystemUtils::calculateSmithPreferencetMatrix(std::vector<std::vector<double>>& ranks)
{
	std::vector<std::vector<int>> concordateMatrix;
	int numberOfCandidates = ranks[0].size();

	for (int i = 0; i < numberOfCandidates; i++)
	{
		concordateMatrix.push_back(std::vector<int>(numberOfCandidates, 0));
	}

	for (auto voter : ranks)
	{
		for (int i = 0; i < voter.size(); i++)
		{
			for (int j = i + 1; j < voter.size(); j++)
			{
				if (voter[i] < voter[j])
				{
					concordateMatrix[i][j] += 1;
				}
				else
				{
					concordateMatrix[j][i] += 1;
				}
			}
		}
	}

	std::vector<std::vector<double>> preferenceMatrix(numberOfCandidates, std::vector<double>(numberOfCandidates, 0));

	for (int i = 0; i < concordateMatrix.size(); i++)
	{
		for (int j = i + 1; j < concordateMatrix.size(); j++)
		{
			if (concordateMatrix[i][j] > concordateMatrix[j][i])
			{
				preferenceMatrix[i][j] = 1;
			}
			else if(concordateMatrix[i][j] == concordateMatrix[j][i])
			{
				preferenceMatrix[i][j] = 0.5;
				preferenceMatrix[j][i] = 0.5;
			}
			else
			{
				preferenceMatrix[j][i] = 1;
			}
		}
	}

	return preferenceMatrix;
}


std::vector<int> ElectoralSystemUtils::sortByScores(std::vector<int>& indices,
	std::vector<double>& scores, std::shared_ptr<WinnerStrategy> winnerStrategy)
{
	int n = indices.size();
	std::vector<int> sortedIndices;

	std::vector <std::pair<double, int>> T(n);

	for (int i = 0; i < n; i++)
	{
		T[i].first = scores[i];
		T[i].second = indices[i];
	}

	sort(T.begin(), T.end(), [&](std::pair<double, int> a, std::pair<double, int> b) -> bool {return winnerStrategy->leftIsBetter(a.first, b.first); });

	int rank = 1, i = 0;
	for (auto t : T)
	{
		sortedIndices.push_back(t.second);
	}

	return sortedIndices;
}

