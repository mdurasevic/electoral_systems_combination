#include "UnrestrictedGlobalRetrievalHeuristicParameters.h"
#include "GlobalRetrievalHeuristicSettings.h"
#include "SelectHighestContainerStrategy.h"
#include "SelectFirstAvailabelContainerStrategy.h"
#include<cmath>

UnrestrictedGlobalRetrievalHeuristicParameters::UnrestrictedGlobalRetrievalHeuristicParameters(shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy, shared_ptr<GlobalRetrievalHeuristicSettings> GRHS)
{
	containerSelection = selectionStrategy;
	this->GRHS = GRHS;
	terminals.setActiveGRHTerminals();
}

void UnrestrictedGlobalRetrievalHeuristicParameters::Solve(ContainerYard& containerYard)
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

			while (containerYard.GetStackHeight(selectedLocation) != 0 && containerYard.GetTopContainerId(selectedLocation) < containerYard.GetTopContainerId(containerLocation)) {

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

			containerYard.Relocate(containerLocation, selectedLocation);
		}
		containerYard.Retrieve(containerLocation);
	}
}
