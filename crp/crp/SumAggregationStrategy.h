#pragma once
#include "EnsembleAggregationStrategy.h"

class SumAggregationStrategy : public EnsembleAggregationStrategy 
{
public:
	std::vector<double> aggregate(std::vector<std::vector<double>>& priorities) override;
};

