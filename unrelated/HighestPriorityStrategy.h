#pragma once
#include "WinnerStrategy.h"
class HighestPriorityStrategy :
    public WinnerStrategy
{
public:
	int winner(int minPriorityIndex, int maxPriorityIndex) override;
	int looser(int minPriorityIndex, int maxPriorityIndex) override;
	bool leftIsBetter(double a, double b) override;
	std::shared_ptr<WinnerStrategy> getInverse() override;
};

