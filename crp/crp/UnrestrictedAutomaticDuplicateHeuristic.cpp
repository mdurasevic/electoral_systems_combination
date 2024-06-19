#include "UnrestrictedAutomaticDuplicateHeuristic.h"

UnrestrictedAutomaticDuplicateHeuristic::UnrestrictedAutomaticDuplicateHeuristic(std::string expression1, std::string expression2, ContainerSelectionType selectionType) : evaluatorSource(terminals), evaluatorDestination(terminals)
{
	this->evaluatorSource.ParseExpression(expression1);
	this->evaluatorDestination.ParseExpression(expression2);
	this->selectionType = selectionType;

}

void UnrestrictedAutomaticDuplicateHeuristic::Solve(ContainerYard& containerYard)
{
	const int bays = containerYard.GetNumberOfBays();
	const int stacks = containerYard.GetNumberOfStacks();
	const int maxHeight = containerYard.GetMaxStackHeight();
	const int filled = bays * stacks * maxHeight;

	while (!containerYard.Empty())
	{
		const int containerId = containerYard.GetNextContainerId();
		Location containerLocation(-1, -1);
		const int containerCount = containerYard.GetNumberOfRemainingContainers();


		auto locations = containerYard.GetContainerLocations(containerId);
		if (selectionType == SELECT_HIGHEST_STRATEGY) {
			if ((filled - containerCount) < maxHeight)
			{
				int selectedHeight = -1;
				Location selectedLocation(-1, -1);
				for (auto location : locations)
				{
					const int height = containerYard.GetHighestContainerPosition(location, containerId);
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
				double bestPriority = DBL_MAX;
				double executionTime = DBL_MAX;
				for (auto location : locations)
				{
					Location craneLocation = containerYard.GetCraneLocation();
					terminals.CalculateTerminals(containerYard, containerYard.GetTopContainerId(location), craneLocation, location);
					evaluatorSource.Reset();
					const double priority = evaluatorSource.Execute();

					if (bestPriority > priority)
					{
						bestPriority = priority;
						containerLocation = location;
					}
					else if (bestPriority == priority)
					{
						const double selectedContainerExecutionTime = containerYard.CalculateTime(craneLocation, containerLocation);
						if (executionTime > selectedContainerExecutionTime)
						{
							bestPriority = priority;
							containerLocation = location;
						}
					}
				}
			}
		}

		else if (selectionType == SELECT_FIRST_AVAILABLE_STRATEGY) {

			double bestPriority = DBL_MAX;
			double executionTime = DBL_MAX;
			for (auto location : locations)
			{
				if ((filled - containerCount) >= (containerYard.GetStackHeight(location) - (containerYard.GetHighestContainerPosition(location, containerId) + 1) + containerYard.GetMaxStackHeight() - containerYard.GetStackHeight(location))) {
					if ((containerYard.GetNumberOfStacksWithEmptyPlaces() == 1) && (containerYard.GetStackHeight(location) < containerYard.GetMaxStackHeight()) && (containerYard.GetTopContainerId(location) != containerId)) {
						continue;
					}

					Location craneLocation = containerYard.GetCraneLocation();
					terminals.CalculateTerminals(containerYard, containerYard.GetTopContainerId(location), craneLocation, location);
					evaluatorSource.Reset();
					const double priority = evaluatorSource.Execute();

					if (bestPriority > priority)
					{
						bestPriority = priority;
						containerLocation = location;
					}
					else if (bestPriority == priority)
					{
						const double selectedContainerExecutionTime = containerYard.CalculateTime(craneLocation, containerLocation);
						if (executionTime > selectedContainerExecutionTime)
						{
							bestPriority = priority;
							containerLocation = location;
						}
					}
				}
			}
		}


		while (containerYard.GetTopContainerId(containerLocation) != containerId)
		{
			double bestPriority = DBL_MAX;
			double executionTime = DBL_MAX;
			Location selectedLocation(-1, -1);

			//cout<<containerYard.GetCurrentYardString()<<endl;


			for (int bay = 0; bay < bays; bay++)
			{
				for (int stack = 0; stack < stacks; stack++)
				{
					Location location = Location(bay, stack);

					if (!containerYard.HasMaximumHeight(location) && location != containerLocation)
					{
						terminals.CalculateTerminals(containerYard, containerYard.GetTopContainerId(containerLocation), containerLocation, location);
						evaluatorDestination.Reset();
						const double priority = evaluatorDestination.Execute();

						//const int selectedContainerHeight = containerYard.GetStackHeight(location);
						const Location& craneLocation = containerYard.GetCraneLocation();
						if (bestPriority > priority)
						{
							bestPriority = priority;
							selectedLocation = location;
							executionTime = containerYard.CalculateTime(craneLocation, selectedLocation);
						}
						else if (bestPriority == priority)
						{
							const double selectedContainerExecutionTime = containerYard.CalculateTime(craneLocation, selectedLocation);
							if (executionTime > selectedContainerExecutionTime)
							{
								bestPriority = priority;
								selectedLocation = location;
								executionTime = selectedContainerExecutionTime;
							}
						}
					}
				}
			}
			/*if(selectedLocation.GetBay()==-1)
			{
				auto locations = containerYard.GetContainerLocations(containerId);

				containerLocation = *std::next(locations.begin(),offset);
				offset++;
				continue;
			}*/

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
			//	std::cout << containerYard->GetCurrentYardString();
		}

		containerYard.Retrieve(containerLocation);
		//std::cout << containerYard->GetCurrentYardString();
	}
}
