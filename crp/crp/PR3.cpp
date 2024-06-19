#include "PR3.h"
#include "SelectHighestContainerStrategy.h"
#include "SelectHighestContainerAlwaysStrategy.h"
#include "SelectMostContainersStrategy.h"
#include "SelectFirstAvailabelContainerStrategy.h"

void PR3::Solve(ContainerYard& containerYard)
{
	int bays = containerYard.GetNumberOfBays();
	int stacks = containerYard.GetNumberOfStacks();
	ContainerSourceSelectionStrategy* containerSelection = new SelectFirstAvailabelContainerStrategy();

	while (!containerYard.Empty())
	{
		int containerId = containerYard.GetNextContainerId();
		Location containerLocation = containerSelection->selectLocation(containerYard, containerId);

		while (containerYard.GetTopContainerId(containerLocation) != containerId)
		{
			int topContainerId = containerYard.GetTopContainerId(containerLocation);
			int lowestIndex = INT_MAX;
			Location selectedLocation(-1, -1);

			int highestIndex = INT_MIN;
			Location selectedLocationHighest(-1, -1);
			for (int bay = 0; bay < bays; bay++)
			{
				for (int stack = 0; stack < stacks; stack++)
				{
					Location location = Location(bay, stack);

					if (location != containerLocation && !containerYard.HasMaximumHeight(location))
					{
						int selectedStackLowestIndex = INT_MAX;

						//Here we find the smallest container ID value for the stack
						for (int height = 0; height < containerYard.GetStackHeight(location); height++)
						{
							int currentContainerId = containerYard.GetContainerId(location, height);
							if(currentContainerId == -1)
							{
								continue;
							}
							if (selectedStackLowestIndex > currentContainerId) 
							{
								selectedStackLowestIndex = currentContainerId;
							}
						}

						if (selectedStackLowestIndex > topContainerId)
						{
							if (lowestIndex > selectedStackLowestIndex)
							{
								lowestIndex = selectedStackLowestIndex;
								selectedLocation = location;
							}
							else if (selectedStackLowestIndex == INT_MAX)
							{
								lowestIndex = 0;
								selectedLocation = location;
							}
						}
						else 
						{
							if(highestIndex<selectedStackLowestIndex)
							{
								highestIndex = selectedStackLowestIndex;
								selectedLocationHighest = location;
							}
						}
					}
				}
			}
			if(lowestIndex<INT_MAX)
			{
				containerYard.Relocate(containerLocation, selectedLocation);
			} else
			{
				containerYard.Relocate(containerLocation, selectedLocationHighest);
			}
		}
		containerYard.Retrieve(containerLocation);
	}
}
