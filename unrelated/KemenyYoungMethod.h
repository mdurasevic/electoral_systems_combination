#pragma once
#include "ElectoralSystem.h"
class KemenyYoungMethod :
    public ElectoralSystem
{
public:
	KemenyYoungMethod(int maximumConsideredCandidates = 7);
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;
private:
	int maximumConsideredCandidates;
};

