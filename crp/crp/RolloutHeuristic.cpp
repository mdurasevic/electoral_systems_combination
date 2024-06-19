#include "RolloutHeuristic.h"

#include "PR4.h"
#include "SelectFirstAvailabelContainerStrategy.h"
#include "TLP.h"

RolloutHeuristic::RolloutHeuristic(std::shared_ptr<PriorityHeuristic> heuristic)
{
	this->heuristic = heuristic;
}

void RolloutHeuristic::Solve(ContainerYard& containerYard)
{
	int bays = containerYard.GetNumberOfBays();
	int stacks = containerYard.GetNumberOfStacks();

	ContainerSourceSelectionStrategy* containerSelection = new SelectFirstAvailabelContainerStrategy();

	while (!containerYard.Empty())
	{
		int containerId = containerYard.GetNextContainerId();
		Location containerLocation(0, 0);
		containerLocation = containerSelection->selectLocation(containerYard, containerId);

		while (containerYard.GetTopContainerId(containerLocation) != containerId)
		{
			Location selectedLocation(-1, -1);
			int selectedMoves = INT_MAX;
			for (int bay = 0; bay < bays; bay++)
			{
				for (int stack = 0; stack < stacks; stack++)
				{
					Location location = Location(bay, stack);

					if (location != containerLocation)
					{
						ContainerYard cy = containerYard;
						cy.Relocate(containerLocation, location);
						heuristic->Solve(cy);
						const int moves = cy.GetNumberOfMoves();
						if(moves<selectedMoves)
						{
							selectedMoves = moves;
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
