#pragma once
#include "Evaluator.h"
#include "PriorityHeuristic.h"

class RestrictedAutomaticHeuristicWithoutNextStack : public PriorityHeuristic
{
private:
	Evaluator evaluator;
	Terminals terminals;

public:
	RestrictedAutomaticHeuristicWithoutNextStack(std::string expression);
	void Solve(ContainerYard& containerYard) override;
};

