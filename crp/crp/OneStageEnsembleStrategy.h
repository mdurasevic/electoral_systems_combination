#pragma once
#include <functional>

#include "EnsembleDecisionStrategy.h"
#include "ContainerYard.h"
#include "EnsembleAggregationStrategy.h"
#include "ElectoralSystem.h"

enum class SelectionType
{
	MINIMUM, MAXIMUM
};

class OneStageEnsembleDecisionStrategy : public EnsembleDecisionStrategy
{
private:
	shared_ptr<ElectoralSystem> electoralSytem;
	SelectionType selectionType;
	shared_ptr<WinnerStrategy> selector;
public:
	OneStageEnsembleDecisionStrategy(shared_ptr<ElectoralSystem> aggregationStrategy, SelectionType selectionType);
	Location PerformDecision(ContainerYard& containerYard, std::vector<Evaluator>& evaluators, Terminals& terminals, Location sourceStack) override;
};


