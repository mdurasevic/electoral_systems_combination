#pragma once
#include "ContainerYard.h"

class PriorityHeuristic
{
public:
	virtual void Solve(ContainerYard& containerYard) = 0;
};

