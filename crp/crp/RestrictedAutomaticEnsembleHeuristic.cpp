#include "RestrictedAutomaticEnsembleHeuristic.h"

RestrictedAutomaticEnsembleHeuristic::RestrictedAutomaticEnsembleHeuristic(const std::vector<std::string>& expressions,
	shared_ptr<ContainerSourceSelectionStrategy> selectionStrategy, shared_ptr<EnsembleDecisionStrategy> strategy)
{
	for(auto expression: expressions)
	{
		Evaluator evaluator = Evaluator(terminals);
		evaluator.ParseExpression(expression);
		evaluators.push_back(evaluator);
	}
	ensembleEvaluationStrategy = strategy;
	containerSelection = selectionStrategy;
}

void RestrictedAutomaticEnsembleHeuristic::Solve(ContainerYard& containerYard)
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
			containerYard.Relocate(containerLocation, selectedLocation);
		}
		containerYard.Retrieve(containerLocation);
	}
}
