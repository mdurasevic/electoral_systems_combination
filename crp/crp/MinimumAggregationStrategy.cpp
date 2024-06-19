#include "MinimumAggregationStrategy.h"

std::vector<double> MinimumAggregationStrategy::aggregate(std::vector<std::vector<double>>& priorities)
{
	std::vector<double> aggregated;

	aggregated.resize(priorities[0].size(), std::numeric_limits<double>::infinity());

	for (int i = 0; i < priorities.size(); i++)
	{
		for (int j = 0; j < priorities[i].size(); j++)
		{
			if(aggregated[j]>priorities[i][j])
			{
				aggregated[j] = priorities[i][j];

			}
		}
	}

	return aggregated;
}
