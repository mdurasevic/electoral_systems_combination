#include "UnrestrictedAutomaticHeuristicWithPrelocationRule.h"

UnrestrictedAutomaticHeuristicWithPrelocationRule::UnrestrictedAutomaticHeuristicWithPrelocationRule(std::string expression, std::string expression2, int prelocations) : evaluator(terminals), evaluator2(terminals)
{
	this->evaluator.ParseExpression(expression);
	this->evaluator2.ParseExpression(expression2);
	this->prelocations = prelocations;

}

void UnrestrictedAutomaticHeuristicWithPrelocationRule::Solve(ContainerYard& containerYard)
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
			while (prelocationMoves)
			{
				prelocationMoves--;
				double bestPriority = DBL_MAX;
				Location sourceLocation(-1, -1);
				
				//select source
				for (int bay = 0; bay < bays; bay++)
				{
					for (int stack = 0; stack < stacks; stack++)
					{
						Location location = Location(bay, stack);

								if (containerYard.GetStackHeight(location) > 0)
								{
									terminals.CalculateTerminals(containerYard, containerYard.GetTopContainerId(location), location, location);
									evaluator.Reset();
									const double priority = evaluator.Execute();

									if (bestPriority > priority)
									{
										bestPriority = priority;
										sourceLocation = location;
									}
						}
					}
				}
				if (sourceLocation.GetStack() == -1)
				{
					break;
				}

				Location destinationLocation(-1, -1);
				bestPriority = DBL_MAX;

				for (int bay = 0; bay < bays; bay++)
				{
					for (int stack = 0; stack < stacks; stack++)
					{
						Location destination = Location(bay, stack);

						if (sourceLocation != destination && !containerYard.HasMaximumHeight(destination) && destination !=containerLocation)
						{
							terminals.CalculateTerminals(containerYard, containerYard.GetTopContainerId(sourceLocation), sourceLocation, destination);
							evaluator2.Reset();
							const double priority = evaluator2.Execute();

							if (bestPriority > priority)
							{
								bestPriority = priority;
								destinationLocation = destination;
							}
						}
					}
				}

				if(destinationLocation.GetStack()==-1)
				{
					break;
				}
				
				containerYard.Relocate(sourceLocation, destinationLocation);
				if (containerYard.GetTopContainerId(containerLocation) == containerId)
				{
					break;
				}
			}
			if (containerYard.GetTopContainerId(containerLocation) == containerId)
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
