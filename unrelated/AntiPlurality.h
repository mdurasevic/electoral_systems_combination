#pragma once
#include "ElectoralSystem.h"
class AntiPlurality :
    public ElectoralSystem
{
public:
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;
};

