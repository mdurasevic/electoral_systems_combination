#include "PR1.h"

void PR1::Solve(ContainerYard& containerYard)
{
	int bays = containerYard.GetNumberOfBays();
	int stacks = containerYard.GetNumberOfStacks();

	while (!containerYard.Empty())
	{
		int containerId = containerYard.GetNextContainerId();
		auto containerLocation = containerYard.GetContainerLocation(containerId);
		while (containerYard.GetTopContainerId(containerLocation) != containerId)
		{
			int stackHeight = INT_MAX;
			double executionTime = DBL_MAX;
			Location selectedLocation(-1, -1);
			for (int bay = 0; bay < bays; bay++)
			{
				for (int stack = 0; stack < stacks; stack++)
				{
					Location location = Location(bay, stack);
					if (location != containerLocation)
					{
						const int selectedContainerHeight = containerYard.GetStackHeight(location);
						if (stackHeight > selectedContainerHeight)
						{
							stackHeight = selectedContainerHeight;
							selectedLocation = location;
						}
					}
				}
			}
			containerYard.Relocate(containerLocation, selectedLocation);
		}

		containerYard.Retrieve(containerLocation);
	}
}
