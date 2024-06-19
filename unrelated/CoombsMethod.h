#pragma once
#include "ElectoralSystem.h"
#include "HighestPriorityStrategy.h"
#include "LowestPriorityStrategy.h"

class CoombsMethod :
    public ElectoralSystem
{
public:
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;
private:
	shared_ptr<LowestPriorityStrategy> lowest = make_shared<LowestPriorityStrategy>();
	shared_ptr<HighestPriorityStrategy> highest = make_shared<HighestPriorityStrategy>();
};

