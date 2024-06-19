#pragma once
#include "PriorityHeuristic.h"
#include "Terminals.h"
#include "Evaluator.h"

class UnrestrictedAutomaticHeuristicWithPrelocationRule :
    public PriorityHeuristic
{
private:
	int prelocations;
	Evaluator evaluator;
	Evaluator evaluator2;

	Terminals terminals;

public:
	UnrestrictedAutomaticHeuristicWithPrelocationRule(std::string expression, std::string expression2, int prelocations = 0);
	void Solve(ContainerYard& containerYard) override;
	
};

