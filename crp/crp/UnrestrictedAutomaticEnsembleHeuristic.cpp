#include "UnrestrictedAutomaticEnsembleHeuristic.h"

UnrestrictedAutomaticEnsembleHeuristic::UnrestrictedAutomaticEnsembleHeuristic(
	const std::vector<std::string>& expressions, shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy,
	shared_ptr<EnsembleDecisionStrategy> strategy)
{
	for (auto expression : expressions)
	{
		Evaluator evaluator = Evaluator(terminals);
		evaluator.ParseExpression(expression);
		evaluators.push_back(evaluator);
	}
	ensembleEvaluationStrategy = strategy;
	containerSelection = selectionStrategy;
}

void UnrestrictedAutomaticEnsembleHeuristic::Solve(ContainerYard& containerYard)
{
	const int bays = containerYard.GetNumberOfBays();
	const int stacks = containerYard.GetNumberOfStacks();

	while (!containerYard.Empty())
	{
		const int containerId = containerYard.GetNextContainerId();
		auto containerLocation = containerSelection->selectLocation(containerYard, containerId);

		while (containerYard.GetTopContainerId(containerLocation) != containerId)
		{
			Location selectedLocation = ensembleEvaluationStrategy->PerformDecision(containerYard, evaluators, terminals, containerLocation);

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
		}
		containerYard.Retrieve(containerLocation);
	}
}
