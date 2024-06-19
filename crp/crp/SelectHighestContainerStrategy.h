#pragma once
#include "ContainerSourceSelectionStrategy.h"

class SelectHighestContainerStrategy : public ContainerSourceSelectionStrategy
{
public:
	Location selectLocation(ContainerYard& yard, int containerId) override;
};

