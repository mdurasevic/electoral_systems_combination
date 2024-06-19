#pragma once
#include "PriorityHeuristic.h"
#include "Evaluator.h"

class UnrestrictedAutomaticHeuristicAllCombinationsTwoTrees :
	public PriorityHeuristic
{
private:
	Evaluator evaluator1;
	Evaluator evaluator2;

	Terminals terminals;
	int prelocations;

public:
	UnrestrictedAutomaticHeuristicAllCombinationsTwoTrees(std::string evaluator1, std::string evaluator2, int prelocations = 0);
	void Solve(ContainerYard& containerYard) override;
};

