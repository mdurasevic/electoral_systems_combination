#include "CombinedApprovalVoting.h"

#include <numeric>

#include "ElectoralSystemUtils.h"
#include "PluralityVote.h"

CombinedApprovalVoting::CombinedApprovalVoting(double approvalRate, double disapprovalRate, bool useApprovalInterval)
{
	this->approvalRate = approvalRate;
	this->disapprovalRate = disapprovalRate;
	this->useApprovalInterval = useApprovalInterval;
}

int CombinedApprovalVoting::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	const int decisionCount = ElectoralSystemUtils::getNumberOfDecisions(priorities);
	vector<int> remainingCandidates = ElectoralSystemUtils::createInitialCandidateList(decisionCount);
	double currentApprovalRate = approvalRate;
	double currentDisapprovalRate = disapprovalRate;

	auto inverseStrategy = winnerStrategy->getInverse();

	do {
		vector<int> votes(decisionCount, 0);
		bool onlyOneCandidate = true;

		for (auto& voter : priorities)
		{
			std::vector<int> approvals;
			if (!useApprovalInterval)
			{
				// in this case there is a fixed percentage of candidates that can be approved
				approvals = ElectoralSystemUtils::determineApprovedCandidates(voter, winnerStrategy, currentApprovalRate);
			}
			else
			{
				// in this case the number of approved candidates can vary because the approval rate is calculated as the range from the best priority
				approvals = ElectoralSystemUtils::determineApprovedCandidatesByInterval(voter, winnerStrategy, currentApprovalRate);
			}
			int approvalCount = 0;
			for (int i = 0; i < approvals.size(); i++)
			{
				approvalCount += approvals[i];
				votes[i] += approvals[i];
			}


			if (approvalCount > 1)
			{
				onlyOneCandidate = false;
			}

			std::vector<int> disapprovals;
			if (!useApprovalInterval)
			{
				// in this case there is a fixed percentage of candidates that can be approved
				disapprovals = ElectoralSystemUtils::determineApprovedCandidates(voter, inverseStrategy, currentDisapprovalRate);
			}
			else
			{
				// in this case the number of approved candidates can vary because the approval rate is calculated as the range from the best priority
				disapprovals = ElectoralSystemUtils::determineApprovedCandidatesByInterval(voter, inverseStrategy, currentDisapprovalRate);
			}
			for (int i = 0; i < disapprovals.size(); i++)
			{
				votes[i] -= disapprovals[i];
			}

		}

			vector<int> winnerIndices = ElectoralSystemUtils::getIndicesWithHighestVoteCount(votes, remainingCandidates);

			// if there is a single winner and we do not want to use a second round. 
			if (winnerIndices.size() == 1)
			{
				return winnerIndices[0];
			}

			// we reduce the approval rate by two and reduce the number of candidates that can be approved by the voters
			currentApprovalRate /= 2;

			// switch to plurality vote in case we vote for only one candidate, or the approval rate becomes too small. 
			if (onlyOneCandidate || currentApprovalRate < 10e-6)
			{
				PluralityVote plurality;
				return plurality.elect(priorities, winnerStrategy);
			}

	} while (true);

}
