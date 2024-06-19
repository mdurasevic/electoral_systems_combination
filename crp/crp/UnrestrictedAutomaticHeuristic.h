#pragma once
#include "PriorityHeuristic.h"
#include "Evaluator.h"
#include "ContainerSourceSelectionStrategy.h"

class UnrestrictedAutomaticHeuristic
	:
	public PriorityHeuristic
{
private:
	Evaluator evaluator;
	Terminals terminals;
	shared_ptr<ContainerSourceSelectionStrategy> containerSelection;
	double retrievalRate;


public:
	UnrestrictedAutomaticHeuristic(std::string expression, shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy, double retrievalRate = 1.0);
	void Solve(ContainerYard& containerYard) override;
};

