#pragma once
#include "PriorityHeuristic.h"

/// <summary>
/// Simple heuristic which works in the same way as TLP, however, it selects the first stack with the lowest height.
/// From Zhu
/// </summary>
class PR1 : public PriorityHeuristic
{
public:
	void Solve(ContainerYard& containerYard) override;
};

