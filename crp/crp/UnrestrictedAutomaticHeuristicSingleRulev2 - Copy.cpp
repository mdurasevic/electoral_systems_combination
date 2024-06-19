#include "UnrestrictedAutomaticHeuristicSingleRulev2.h"

UnrestrictedAutomaticHeuristicSingleRulev2::UnrestrictedAutomaticHeuristicSingleRulev2(std::string expression) : evaluator(terminals)
{
	this->evaluator.ParseExpression(expression);

}

void UnrestrictedAutomaticHeuristicSingleRulev2::Solve(ContainerYard& containerYard)
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
						terminals.CalculateTerminals(containerYard, location);
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
					}
				}
			}

			while (1) {
				if (containerYard.GetStackHeight(selectedLocation) != 0 && containerYard.GetTopContainerId(selectedLocation) < containerYard.GetTopContainerId(containerLocation))
				{
					Location selectedLocationLow(-1, -1);
					int lowestIndex = INT_MIN;
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
									if (lowestIndex < containerYard.GetTopContainerId(selectedLocation))
									{
										lowestIndex = containerYard.GetTopContainerId(selectedLocation);
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
