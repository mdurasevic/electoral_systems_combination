#pragma once
#include "ContainerSourceSelectionStrategy.h"

class SelectMostContainersStrategy : public ContainerSourceSelectionStrategy
{
public:
	virtual Location selectLocation(ContainerYard& yard, int containerId) override;
};

