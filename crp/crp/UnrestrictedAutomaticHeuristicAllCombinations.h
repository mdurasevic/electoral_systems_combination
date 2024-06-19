#pragma once
#include "PriorityHeuristic.h"
#include "Terminals.h"
#include "Evaluator.h"

class UnrestrictedAutomaticHeuristicAllCombinations :
	public PriorityHeuristic
{
private:
	Evaluator evaluator;
	Terminals terminals;
	int prelocations;

public:
	UnrestrictedAutomaticHeuristicAllCombinations(std::string evaluator, int prelocations = 0);
	void Solve(ContainerYard& containerYard) override;
};

