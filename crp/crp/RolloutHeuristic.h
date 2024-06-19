#pragma once
#include "PriorityHeuristic.h"

class RolloutHeuristic : public PriorityHeuristic
{
public:
	RolloutHeuristic(std::shared_ptr<PriorityHeuristic> heuristic);
	void Solve(ContainerYard& containerYard) override;
private:
	std::shared_ptr<PriorityHeuristic> heuristic;
};

