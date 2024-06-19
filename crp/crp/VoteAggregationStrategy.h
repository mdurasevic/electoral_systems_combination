#pragma once
#include "EnsembleAggregationStrategy.h"

class VoteAggregationStrategy : public EnsembleAggregationStrategy
{
public:
	std::vector<double> aggregate(std::vector<std::vector<double>>& priorities) override;
};

