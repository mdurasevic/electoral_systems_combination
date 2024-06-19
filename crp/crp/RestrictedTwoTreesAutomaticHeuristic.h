#pragma once
#include "PriorityHeuristic.h"
#include "Terminals.h"
#include "Evaluator.h"

class RestrictedTwoTreesAutomaticHeuristic :
	public PriorityHeuristic
{
private:
	Evaluator evaluator1;
	Evaluator evaluator2;
	Terminals terminals;

public:
	RestrictedTwoTreesAutomaticHeuristic(std::string expression1, std::string expression2);
	void Solve(ContainerYard& containerYard) override;
};

