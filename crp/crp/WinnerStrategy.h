#pragma once
#include <memory>
#include <utility>

class WinnerStrategy
{
public:
	virtual int winner(int minPriorityIndex, int maxPriorityIndex) = 0;
	virtual int looser(int minPriorityIndex, int maxPriorityIndex) = 0;
	virtual bool leftIsBetter(double a, double b) = 0;
	virtual std::shared_ptr<WinnerStrategy> getInverse() = 0;
};

