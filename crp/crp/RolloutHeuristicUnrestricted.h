#pragma once
#include "PriorityHeuristic.h"

class RolloutHeuristicUnrestricted :
    public PriorityHeuristic
{
public:
	RolloutHeuristicUnrestricted(std::shared_ptr<PriorityHeuristic> heuristic);
	void Solve(ContainerYard& containerYard) override;
private:
	std::shared_ptr<PriorityHeuristic> heuristic;
};

