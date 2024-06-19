#pragma once
#include <vector>

class EnsembleAggregationStrategy
{
public:
	virtual std::vector<double> aggregate(std::vector<std::vector<double>>& priorities);
};

