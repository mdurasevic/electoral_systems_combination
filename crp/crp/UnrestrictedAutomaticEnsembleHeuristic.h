#pragma once
#include "ContainerSourceSelectionStrategy.h"
#include "EnsembleDecisionStrategy.h"
#include "PriorityHeuristic.h"
class UnrestrictedAutomaticEnsembleHeuristic :
    public PriorityHeuristic
{

private:
	std::vector<Evaluator> evaluators;
	Terminals terminals;
	shared_ptr<ContainerSourceSelectionStrategy> containerSelection;
	shared_ptr<EnsembleDecisionStrategy> ensembleEvaluationStrategy;
public:
	UnrestrictedAutomaticEnsembleHeuristic(const std::vector<std::string>& expressions, shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy, shared_ptr<EnsembleDecisionStrategy> strategy);
	void Solve(ContainerYard& containerYard) override;
};

