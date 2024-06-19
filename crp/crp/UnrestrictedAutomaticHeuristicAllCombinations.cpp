#include "UnrestrictedAutomaticHeuristicAllCombinations.h"

UnrestrictedAutomaticHeuristicAllCombinations::UnrestrictedAutomaticHeuristicAllCombinations(
	std::string evaluator, int prelocations) :evaluator(terminals)
{
	this->prelocations = prelocations;
	this->evaluator.ParseExpression(evaluator);
}

void UnrestrictedAutomaticHeuristicAllCombinations::Solve(ContainerYard& containerYard)
{

	const int bays = containerYard.GetNumberOfBays();
	const int stacks = containerYard.GetNumberOfStacks();

	while (!containerYard.Empty())
	{
		const int containerId = containerYard.GetNextContainerId();
		auto containerLocation = containerYard.GetContainerLocation(containerId);
		
		//auto containerLocation = *containerLocations.begin();

		
		while (containerYard.GetTopContainerId(containerLocation) != containerId)
		{
			int prelocationMoves = prelocations;
			while (prelocationMoves--)
			{
				prelocationMoves--;
				double bestPriority = DBL_MAX;
				Location sourceLocation(-1, -1);
				Location destinationLocation(-1, -1);
				//select source
				for (int bay = 0; bay < bays; bay++)
				{
					for (int stack = 0; stack < stacks; stack++)
					{
						Location location = Location(bay, stack);
						for (int bay1 = 0; bay < bays; bay++)
						{
							for (int stack2 = 0; stack < stacks; stack++)
							{
								Location destination = Location(bay1, stack2);

								if (containerYard.GetStackHeight(location) > 0 && location != destination && !containerYard.HasMaximumHeight(destination))
								{
									terminals.CalculateTerminals(containerYard, containerYard.GetTopContainerId(location), location, destination);
									evaluator.Reset();
									const double priority = evaluator.Execute();

									if (bestPriority > priority)
									{
										bestPriority = priority;
										sourceLocation = location;
										destinationLocation = destination;
									}
								}
							}
						}
					}
				}
				if (sourceLocation.GetStack() == -1 )
				{
					break;
				}
				containerYard.Relocate(sourceLocation, destinationLocation);
				if(containerYard.GetTopContainerId(containerLocation) == containerId)
				{
					break;
				}
			}
			 if(containerYard.GetTopContainerId(containerLocation) == containerId)
			 {
				 break;
			 }
			
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


			containerYard.Relocate(containerLocation, selectedLocation);
			//	std::cout << containerYard->GetCurrentYardString();
		}

		containerYard.Retrieve(containerLocation);
		//std::cout << containerYard->GetCurrentYardString();
	}
}
