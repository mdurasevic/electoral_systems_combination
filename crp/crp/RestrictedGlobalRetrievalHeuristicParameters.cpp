#include "RestrictedGlobalRetrievalHeuristicParameters.h"
#include "GlobalRetrievalHeuristicSettings.h"
#include "SelectHighestContainerStrategy.h"
#include "SelectFirstAvailabelContainerStrategy.h"
#include<cmath>

RestrictedGlobalRetrievalHeuristicParameters::RestrictedGlobalRetrievalHeuristicParameters(shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy, shared_ptr<GlobalRetrievalHeuristicSettings> GRHS)
{
	containerSelection = selectionStrategy;
	this->GRHS = GRHS;
	terminals.setActiveGRHTerminals();
}

void RestrictedGlobalRetrievalHeuristicParameters::Solve(ContainerYard& containerYard)
{
	const int bays = containerYard.GetNumberOfBays();
	const int stacks = containerYard.GetNumberOfStacks();
	while (!containerYard.Empty())
	{
		const int containerId = containerYard.GetNextContainerId();
		auto containerLocation = containerSelection->selectLocation(containerYard, containerId);
		while (containerYard.GetTopContainerId(containerLocation) != containerId)
		{
			double smallestPenalty = DBL_MAX;
			double executionTime = DBL_MAX;
			Location selectedLocation(-1, -1);
			for (int bay = 0; bay < bays; bay++)
			{
				for (int stack = 0; stack < stacks; stack++)
				{
					Location location = Location(bay, stack);
					int containerToRelocate = containerYard.GetTopContainerId(containerLocation);
					if (!containerYard.HasMaximumHeight(location) && location != containerLocation)
					{
						terminals.CalculateTerminals(containerYard, containerToRelocate, containerLocation, location);
						std::vector<double> params = GRHS->getGRHSettings();


						const double penalty = params[0] * terminals.GetTerminalValue(HS)
							+ params[1] * terminals.GetTerminalValue(LS)
							+ params[2] * terminals.GetTerminalValue(DIS)
							+ params[3] * terminals.GetTerminalValue(RS)
							+ params[4] * terminals.GetTerminalValue(MIN)
							+ params[5] * terminals.GetTerminalValue(GS)
							+ params[6] * terminals.GetTerminalValue(KS)
							+ params[7] * terminals.GetTerminalValue(SH);

						const Location& craneLocation = containerYard.GetCraneLocation();
						if (smallestPenalty > penalty)
						{
							smallestPenalty = penalty;
							selectedLocation = location;
							executionTime = containerYard.CalculateTime(craneLocation, selectedLocation);
						}
						else if (smallestPenalty == penalty)
						{
							const double selectedContainerExecutionTime = containerYard.CalculateTime(craneLocation, selectedLocation);
							if (executionTime > selectedContainerExecutionTime)
							{
								smallestPenalty = penalty;
								selectedLocation = location;
								executionTime = selectedContainerExecutionTime;
							}
						}
					}
				}
			}

			containerYard.Relocate(containerLocation, selectedLocation);
		}
		containerYard.Retrieve(containerLocation);
	}
}
