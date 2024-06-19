#pragma once
#include "ContainerSourceSelectionStrategy.h"

class SelectFirstAvailabelContainerStrategy : public ContainerSourceSelectionStrategy
{
	Location selectLocation(ContainerYard& yard, int containerId) override;
};

