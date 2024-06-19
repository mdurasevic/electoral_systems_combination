#pragma once
#include "PriorityHeuristic.h"
#include "Evaluator.h"
#include "ContainerSourceSelectionStrategy.h"
#include "GlobalRetrievalHeuristicSettings.h"


class UnrestrictedGlobalRetrievalHeuristicParameters :
	public PriorityHeuristic
{
private:
	Terminals terminals;
	shared_ptr<ContainerSourceSelectionStrategy> containerSelection;
	shared_ptr<GlobalRetrievalHeuristicSettings> GRHS;

public:
	UnrestrictedGlobalRetrievalHeuristicParameters(shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy, shared_ptr<GlobalRetrievalHeuristicSettings> GRHS);
	void Solve(ContainerYard& containerYard) override;
};

