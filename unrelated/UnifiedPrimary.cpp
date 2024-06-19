#include "UnifiedPrimary.h"

#include <numeric>

#include "ElectoralSystemUtils.h"

UnifiedPrimary::UnifiedPrimary(double approvalRate, bool useApprovalInterval)
{
	this->approvalRate = approvalRate;
	this->useApprovalInterval = useApprovalInterval;
}

int UnifiedPrimary::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	vector<int> votes(decisionCount, 0);
	
	for (auto& voter : priorities)
	{
		std::vector<int> approvals;
		if (!useApprovalInterval)
		{
			// in this case there is a fixed percentage of candidates that can be approved
			approvals = ElectoralSystemUtils::determineApprovedCandidates(voter, winnerStrategy, approvalRate);
		}
		else
		{
			// in this case the number of approved candidates can vary because the approval rate is calculated as the range from the best priority
			approvals = ElectoralSystemUtils::determineApprovedCandidatesByInterval(voter, winnerStrategy, approvalRate);
		}
		for (int i = 0; i < approvals.size(); i++)
		{
			votes[i] += approvals[i];
		}
	}

	vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, remainingCandidates);

	if(winnerIndices.size()==1)
	{
		vector<int> secondBest = ElectoralSystemUtils::determineSecondBestCandidates(votes, winnerIndices[0]);
		for (int second_best : secondBest)
		{
			winnerIndices.push_back(second_best);
		}
	}

	vector<int> secondRoundVotes(decisionCount, 0);
	for (auto& voter : priorities)
	{
		int winnerIndex = ElectoralSystemUtils::determineVoteFromSubset(voter, winnerStrategy, winnerIndices);
		secondRoundVotes[winnerIndex]++;
	}
	auto newWinnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(secondRoundVotes, winnerIndices);
	return newWinnerIndices[0];
}
