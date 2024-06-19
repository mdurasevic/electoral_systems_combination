#include "UnrestrictedAutomaticTwoTreesHeuristic.h"

UnrestrictedAutomaticTwoTreesHeuristic::UnrestrictedAutomaticTwoTreesHeuristic(std::string expression, std::string expression2, int prelocations) : evaluator(terminals), evaluator2(terminals)
{
	this->evaluator.ParseExpression(expression);
	this->evaluator2.ParseExpression(expression2);

	this->prelocations = prelocations;
}

void UnrestrictedAutomaticTwoTreesHeuristic::Solve(ContainerYard& containerYard)
{
	const int bays = containerYard.GetNumberOfBays();
	const int stacks = containerYard.GetNumberOfStacks();

	while (!containerYard.Empty())
	{
		const int containerId = containerYard.GetNextContainerId();
		auto containerLocation = containerYard.GetContainerLocation(containerId);

		while (containerYard.GetTopContainerId(containerLocation) != containerId)
		{
			double bestPriority = DBL_MAX;
			double executionTime = DBL_MAX;
			Location selectedLocation(-1, -1);
			for (int bay = 0; bay < bays; bay++)
			{
				for (int stack = 0; stack < stacks; stack++)
				{
					Location location = Location(bay, stack);

					if (!containerYard.HasMaximumHeight(location) && location != containerLocation)
					{
						terminals.CalculateTerminals(containerYard, containerYard.GetTopContainerId(containerLocation), containerLocation, location);
						evaluator.Reset();
						const double priority = evaluator.Execute();

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

			for (int bay = 0; bay < bays; bay++)
			{
				for (int stack = 0; stack < stacks; stack++)
				{
					Location location = Location(bay, stack);

					if (!containerYard.HasMaximumHeight(location) && location != containerLocation)
					{
						terminals.CalculateTerminals(containerYard, containerYard.GetTopContainerId(containerLocation), containerLocation, location);
						evaluator2.Reset();
						const double priority = evaluator2.Execute();

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
