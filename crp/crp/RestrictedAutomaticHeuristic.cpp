#include "RestrictedAutomaticHeuristic.h"

#include <iostream>

#include "SelectHighestContainerStrategy.h"
#include "SelectFirstAvailabelContainerStrategy.h"

RestrictedAutomaticHeuristic::RestrictedAutomaticHeuristic(std::string expression, shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy, double retrievalRate) : evaluator(terminals)
{
	this->evaluator.ParseExpression(expression);
	containerSelection = selectionStrategy;
	this->retrievalRate = retrievalRate;
}

void RestrictedAutomaticHeuristic::Solve(ContainerYard& containerYard)
{
	const int bays = containerYard.GetNumberOfBays();
	const int stacks = containerYard.GetNumberOfStacks();

	while (!containerYard.Empty())
	{
		const int containerId = containerYard.GetNextContainerId();
		auto containerLocation = containerSelection->selectLocation(containerYard, containerId);

		int countersToRetrieved = containerYard.GetNumberOfContainers() * retrievalRate;

		while (containerYard.GetTopContainerId(containerLocation) != containerId && containerYard.GetNumberOFRetrieved() < countersToRetrieved)
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


			containerYard.Relocate(containerLocation, selectedLocation);
				//std::cout << containerYard.GetCurrentYardString();
				//std::cout << "**********************************************"<<endl;
		}

		containerYard.Retrieve(containerLocation);
		//std::cout << containerYard.GetCurrentYardString();
		//std::cout << "**********************************************"<<endl;
	}
}
