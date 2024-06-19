#include "HighestPriorityStrategy.h"

#include <memory>

#include "LowestPriorityStrategy.h"

int HighestPriorityStrategy::winner(int minPriorityIndex, int maxPriorityIndex)
{
	return maxPriorityIndex;
}

int HighestPriorityStrategy::looser(int minPriorityIndex, int maxPriorityIndex)
{
	return minPriorityIndex;
}

bool HighestPriorityStrategy::leftIsBetter(double a, double b)
{
	return a > b;
}

std::shared_ptr<WinnerStrategy> HighestPriorityStrategy::getInverse()
{
	return std::make_shared<LowestPriorityStrategy>();
}
