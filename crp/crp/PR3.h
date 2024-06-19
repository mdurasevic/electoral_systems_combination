#pragma once
#include "PriorityHeuristic.h"
class PR3 :
    public PriorityHeuristic
{
public:
	void Solve(ContainerYard& containerYard) override;
};

