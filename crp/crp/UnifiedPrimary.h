#pragma once
#include "ElectoralSystem.h"
class UnifiedPrimary :
    public ElectoralSystem
{
public:
	UnifiedPrimary(double approvalRate = 0.5, bool useApprovalInterval = false);
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;

private:
	double approvalRate;
	bool useApprovalInterval;
};

