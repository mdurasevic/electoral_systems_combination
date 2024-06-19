#pragma once
#include "EnsembleAggregationStrategy.h"

class MinimumAggregationStrategy : public EnsembleAggregationStrategy 
{
public:
	std::vector<double> aggregate(std::vector<std::vector<double>>& priorities) override;
};

