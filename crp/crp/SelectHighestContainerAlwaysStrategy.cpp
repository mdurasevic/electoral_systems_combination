#include "SelectHighestContainerAlwaysStrategy.h"

Location SelectHighestContainerAlwaysStrategy::selectLocation(ContainerYard& yard, int containerId)
{
	Location containerLocation(-1, -1);
	if (yard.HasDuplicates())
	{
		auto locations = yard.GetContainerLocations(containerId);
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
		return this->getSingleSourceLocation(yard, containerId);
	}
	return containerLocation;
}
