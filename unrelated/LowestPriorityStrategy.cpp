#include "LowestPriorityStrategy.h"

#include <memory>

#include "HighestPriorityStrategy.h"

int LowestPriorityStrategy::winner(int minPriorityIndex, int maxPriorityIndex)
{
	return minPriorityIndex;
}

int LowestPriorityStrategy::looser(int minPriorityIndex, int maxPriorityIndex)
{
	return maxPriorityIndex;
}

bool LowestPriorityStrategy::leftIsBetter(double a, double b)
{
	return a < b;
}

std::shared_ptr<WinnerStrategy> LowestPriorityStrategy::getInverse()
{
	return std::make_shared<HighestPriorityStrategy>();
}
