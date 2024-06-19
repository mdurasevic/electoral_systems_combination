#pragma once
#include "PriorityHeuristic.h"
#include "ContainerSourceSelectionStrategy.h"
#include "Evaluator.h"
#include "ContainerSelectionType.h"


class UnrestrictedAutomaticDuplicateHeuristic : public PriorityHeuristic
{
private:
	Evaluator evaluatorSource;
	Evaluator evaluatorDestination;
	Terminals terminals;
	ContainerSelectionType selectionType;


public:
	UnrestrictedAutomaticDuplicateHeuristic(std::string expression1, std::string expression2, ContainerSelectionType selectionType);
	void Solve(ContainerYard& containerYard) override;
};

