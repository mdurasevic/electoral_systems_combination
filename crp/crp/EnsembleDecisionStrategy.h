#pragma once
#include "ContainerYard.h"
#include "Evaluator.h"

class EnsembleDecisionStrategy
{
public:
	virtual Location PerformDecision(ContainerYard& yard, std::vector<Evaluator>& evaluators, Terminals& terminals, Location sourceStack) = 0;
};

