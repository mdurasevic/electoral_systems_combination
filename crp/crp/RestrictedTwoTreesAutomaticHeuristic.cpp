#include "RestrictedTwoTreesAutomaticHeuristic.h"

RestrictedTwoTreesAutomaticHeuristic::RestrictedTwoTreesAutomaticHeuristic(std::string expression1, std::string expression2) : evaluator1(terminals), evaluator2(terminals)
{
	this->evaluator1.ParseExpression(expression1);
	this->evaluator2.ParseExpression(expression2);
}

void RestrictedTwoTreesAutomaticHeuristic::Solve(ContainerYard& containerYard)
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
						evaluator1.Reset();
						const double priority = evaluator1.Execute();

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


			containerYard.Relocate(containerLocation, selectedLocation);
			//	std::cout << containerYard->GetCurrentYardString();
		}
	
		containerYard.Retrieve(containerLocation);
			//std::cout << containerYard->GetCurrentYardString();
	}
}

