#pragma once
#include "PriorityHeuristic.h"
#include "Evaluator.h"
#include "ContainerSourceSelectionStrategy.h"
#include "EnsembleDecisionStrategy.h"

class RestrictedAutomaticEnsembleHeuristic : public PriorityHeuristic
{
private:
	std::vector<Evaluator> evaluators;
	Terminals terminals;
	shared_ptr<ContainerSourceSelectionStrategy> containerSelection;
	shared_ptr<EnsembleDecisionStrategy> ensembleEvaluationStrategy;

public:
	RestrictedAutomaticEnsembleHeuristic(const std::vector<std::string>& expressions, shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy, shared_ptr<EnsembleDecisionStrategy> strategy);
	void Solve(ContainerYard& containerYard) override;
};

