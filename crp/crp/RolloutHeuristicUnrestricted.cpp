#include "RolloutHeuristicUnrestricted.h"

#include "SelectFirstAvailabelContainerStrategy.h"

RolloutHeuristicUnrestricted::RolloutHeuristicUnrestricted(std::shared_ptr<PriorityHeuristic> heuristic)
{
	this->heuristic = heuristic;
}

void RolloutHeuristicUnrestricted::Solve(ContainerYard& containerYard)
{
	const int bays = containerYard.GetNumberOfBays();
	const int stacks = containerYard.GetNumberOfStacks();
	ContainerSourceSelectionStrategy* containerSelection = new SelectFirstAvailabelContainerStrategy();

	while (!containerYard.Empty())
	{
		const int containerId = containerYard.GetNextContainerId();
		auto containerLocation = containerSelection->selectLocation(containerYard, containerId);

		while (containerYard.GetTopContainerId(containerLocation) != containerId)
		{
			Location selectedLocation(-1, -1);
			int selectedMoves = INT_MAX;

			for (int bay = 0; bay < bays; bay++)
			{
				for (int stack = 0; stack < stacks; stack++)
				{
					Location currentLocation = Location(bay, stack);

					if (!containerYard.HasMaximumHeight(currentLocation) && currentLocation != containerLocation)
					{
						ContainerYard cy = containerYard;

						while (1) {
							if (cy.GetStackHeight(currentLocation) != 0 && cy.GetTopContainerId(currentLocation) < cy.GetTopContainerId(containerLocation))
							{
								Location selectedLocationLow(-1, -1);
								int lowestIndex = INT_MAX;
								for (int bay = 0; bay < bays; bay++)
								{
									for (int stack = 0; stack < stacks; stack++)
									{
										Location location = Location(bay, stack);
										if (!cy.HasMaximumHeight(location) && location != containerLocation && location != currentLocation)
										{
											int selectedStackLowestIndex = INT_MAX;
											for (int height = 0; height < cy.GetStackHeight(location); height++)
											{
												int currentContainerId = cy.GetContainerId(location, height);
												if (selectedStackLowestIndex > currentContainerId)
												{
													selectedStackLowestIndex = currentContainerId;
												}
											}
											if (selectedStackLowestIndex > cy.GetTopContainerId(currentLocation))
											{
												if (lowestIndex > selectedStackLowestIndex)
												{
													lowestIndex = selectedStackLowestIndex;
													selectedLocationLow = location;
												}
												else if (selectedStackLowestIndex == INT_MAX)
												{
													lowestIndex = 0;
													selectedLocationLow = location;
												}
											}
										}


									}
								}
								if (selectedLocationLow != Location(-1, -1))
								{
									cy.Relocate(currentLocation, selectedLocationLow);
								}
								else
								{
									break;
								}
							}
							else
							{
								break;
							}
						}

						cy.Relocate(containerLocation, currentLocation);


						heuristic->Solve(cy);
						const int moves = cy.GetNumberOfMoves();
						if (moves < selectedMoves)
						{
							selectedMoves = moves;
							selectedLocation = currentLocation;
						}
					}
				}
			}


			while (1) {
				if (containerYard.GetStackHeight(selectedLocation) != 0 && containerYard.GetTopContainerId(selectedLocation) < containerYard.GetTopContainerId(containerLocation))
				{
					Location selectedLocationLow(-1, -1);
					int lowestIndex = INT_MAX;
					for (int bay = 0; bay < bays; bay++)
					{
						for (int stack = 0; stack < stacks; stack++)
						{
							Location location = Location(bay, stack);
							if (!containerYard.HasMaximumHeight(location) && location != containerLocation && location != selectedLocation)
							{
								int selectedStackLowestIndex = INT_MAX;
								for (int height = 0; height < containerYard.GetStackHeight(location); height++)
								{
									int currentContainerId = containerYard.GetContainerId(location, height);
									if (selectedStackLowestIndex > currentContainerId)
									{
										selectedStackLowestIndex = currentContainerId;
									}
								}
								if (selectedStackLowestIndex > containerYard.GetTopContainerId(selectedLocation))
								{
									if (lowestIndex > selectedStackLowestIndex)
									{
										lowestIndex = selectedStackLowestIndex;
										selectedLocationLow = location;
									}
									else if (selectedStackLowestIndex == INT_MAX)
									{
										lowestIndex = 0;
										selectedLocationLow = location;
									}
								}
							}


						}
					}
					if (selectedLocationLow != Location(-1, -1))
					{
						containerYard.Relocate(selectedLocation, selectedLocationLow);
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			containerYard.Relocate(containerLocation, selectedLocation);
		}

		containerYard.Retrieve(containerLocation);
	}
}
