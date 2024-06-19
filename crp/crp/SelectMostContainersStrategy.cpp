#include "SelectMostContainersStrategy.h"

Location SelectMostContainersStrategy::selectLocation(ContainerYard& yard, int containerId)
{

	Location containerLocation(-1, -1);

	int bays = yard.GetNumberOfBays();
	int stacks = yard.GetNumberOfStacks();
	const int maxHeight = yard.GetMaxStackHeight();
	const int filled = bays * stacks * maxHeight;
	const int containerCount = yard.GetNumberOfRemainingContainers();


	if (yard.HasDuplicates())
	{
		auto locations = yard.GetContainerLocations(containerId);
		if ((filled - containerCount) < maxHeight)
		{
			int selectedHeight = -1;
			Location selectedLocation(-1, -1);
			for (auto location : locations)
			{
				int height = yard.GetHighestContainerPosition(location, containerId);
				if (height > selectedHeight)
				{
					selectedHeight = height;
					selectedLocation = location;
				}
			}
			containerLocation = selectedLocation;
		}
		else
		{
			int maxContainers = 0;
			Location selectedLocation(-1,-1);
			for (auto location : locations)
			{
				int containersOnLocation = 0;
				for (int i = 0; i < yard.GetStackHeight(location); i++)
				{
					if (yard.GetContainerId(location, i) == containerId)
					{
						containersOnLocation++;
					}
				}
				if (containersOnLocation > maxContainers)
				{
					maxContainers = containersOnLocation;
					selectedLocation = location;
				}
			}
			containerLocation = selectedLocation;
		}
	}
	else
	{
		return this->getSingleSourceLocation(yard, containerId);
	}
	return containerLocation;
}
