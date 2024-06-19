#pragma once
#include "ElectoralSystem.h"
class MiniMax :
    public ElectoralSystem
{
public:
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;
};

