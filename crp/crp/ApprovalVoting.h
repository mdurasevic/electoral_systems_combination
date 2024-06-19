#pragma once
#include "ElectoralSystem.h"

class ApprovalVoting : public ElectoralSystem 
{
public:
	ApprovalVoting(double approvalRate = 0.5, bool useApprovalRange = false, bool useSecondRound = true);
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;

private:
	double approvalRate;
	bool useSecondRound;
	bool useApprovalRange;
};

