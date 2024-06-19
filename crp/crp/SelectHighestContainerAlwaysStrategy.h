#pragma once 
#include "ContainerSourceSelectionStrategy.h"

class SelectHighestContainerAlwaysStrategy : public ContainerSourceSelectionStrategy
{
public:
	Location selectLocation(ContainerYard& yard, int containerId) override;
};

