#include "HighestPriorityStrategy.h"
#include "LowestPriorityStrategy.h"
#include "OneStageEnsembleStrategy.h"

OneStageEnsembleDecisionStrategy::OneStageEnsembleDecisionStrategy(
	shared_ptr<ElectoralSystem> aggregationStrategy, SelectionType selectionType): electoralSytem(aggregationStrategy), selectionType(selectionType)
{
	if (selectionType == SelectionType::MAXIMUM)
	{
		selector = make_shared<HighestPriorityStrategy>();
	}
	else
	{
		selector = make_shared<LowestPriorityStrategy>();
	}
}

Location OneStageEnsembleDecisionStrategy::PerformDecision(ContainerYard& containerYard, std::vector<Evaluator>& evaluators, Terminals& terminals, Location sourceStack)
{
	const int bays = containerYard.GetNumberOfBays();
	const int stacks = containerYard.GetNumberOfStacks();

	std::vector<std::vector<double>> priorities;
	std::vector<Location> locations;
	priorities.resize(evaluators.size());


	for (int bay = 0; bay < bays; bay++)
	{
		for (int stack = 0; stack < stacks; stack++)
		{
			Location location = Location(bay, stack);
			terminals.CalculateTerminals(containerYard, containerYard.GetTopContainerId(sourceStack), sourceStack, location);

			if (!containerYard.HasMaximumHeight(location) && location != sourceStack)
			{
				locations.push_back(location);
				for (int i = 0; i < evaluators.size(); i++) {
					evaluators[i].Reset();
					const double priority = evaluators[i].Execute();
					priorities[i].push_back(priority);

				}
			}
		}
	}


	int winnerIndex = electoralSytem->elect(priorities, selector);

	return locations[winnerIndex];
}
