#pragma once
#include "ElectoralSystem.h"
class CombinedApprovalVoting :
    public ElectoralSystem
{
public:
	CombinedApprovalVoting(double approvalRate = 0.33, double disapprovalRate = 0.33, bool useApprovalInterval = false);
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;

private:
	double approvalRate;
	double disapprovalRate;
	bool useApprovalInterval;
};

