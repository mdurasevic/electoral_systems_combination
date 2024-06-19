#include "SumAggregationStrategy.h"

std::vector<double> SumAggregationStrategy::aggregate(std::vector<std::vector<double>>& priorities)
{
	std::vector<double> aggregated;

	aggregated.resize(priorities[0].size(), 0);

	for(int i = 0; i<priorities.size(); i++)
	{
		for(int j=0;j<priorities[i].size();j++)
		{
			aggregated[j] += priorities[i][j];
		}
	}

	return aggregated;
}
