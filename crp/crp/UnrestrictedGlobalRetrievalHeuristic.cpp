#include "UnrestrictedGlobalRetrievalHeuristic.h"
#include "GlobalRetrievalHeuristicSettings.h"
#include "SelectHighestContainerStrategy.h"
#include "SelectFirstAvailabelContainerStrategy.h"
#include<cmath>

void UnrestrictedGlobalRetrievalHeuristic::fitToRange(vector<double>& values)
{
	for (int i = 0; i < values.size(); i++)
	{
		if (values[i] < 0)
		{
			values[i] = 0;
		}
		else if (values[i] > 1)
		{
			values[i] = 1;
		}
	}
}

UnrestrictedGlobalRetrievalHeuristic::UnrestrictedGlobalRetrievalHeuristic(shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy, shared_ptr<GlobalRetrievalHeuristicSettings> GRHS)
{
	containerSelection = selectionStrategy;
	this->GRHS = GRHS;
	terminals.setActiveGRHTerminals();
}

void UnrestrictedGlobalRetrievalHeuristic::Solve(ContainerYard& containerYard)
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
						double w_max = containerYard.getMaximumWeightOfContainers();
						std::vector<double> params = GRHS->getGRHSettings();

						fitToRange(params);


						double A1 = 1 + (containerYard.getContainerWeight(containerToRelocate) / w_max) * 10 * params[GRHSettings::P1];
						double A3 = 1 + (containerYard.getContainerWeight(containerToRelocate) / w_max) * 10 * params[GRHSettings::P3];
						double A4 = 1 + (containerYard.getContainerWeight(containerToRelocate) / w_max) * 10 * params[GRHSettings::P4];

						const double penalty = params[GRHSettings::alpha] * pow(terminals.GetTerminalValue(HS) / containerYard.GetMaxStackHeight(), A1)
							+ params[GRHSettings::beta] * pow(terminals.GetTerminalValue(LS) / containerYard.GetMaxStackHeight(), A1)
							+ params[GRHSettings::gamma] * pow(terminals.GetTerminalValue(DIS) / containerYard.GetNumberOfStacks(), A1)
							+ params[GRHSettings::delta] * terminals.GetTerminalValue(RS) * pow(containerYard.getContainerWeight(containerToRelocate) / w_max, 10 * params[GRHSettings::P2])
							+ params[GRHSettings::epsilon] * terminals.GetTerminalValue(RS) * pow((containerToRelocate - terminals.GetTerminalValue(MIN)) / containerYard.GetNumberOfContainers(), (int)round(A3))
							+ params[GRHSettings::eta] * (1 - terminals.GetTerminalValue(RS)) * terminals.GetTerminalValue(GS)
							+ params[GRHSettings::theta] * pow(terminals.GetTerminalValue(KS) / containerYard.GetNumberOfStacks(), A4)
							+ params[GRHSettings::mi] * (terminals.GetTerminalValue(SH) / containerYard.GetMaxStackHeight());

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
