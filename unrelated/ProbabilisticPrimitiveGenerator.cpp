#include "ProbabilisticPrimitiveGenerator.h"

#include <iostream>

ProbabilisticPrimitiveGenerator::ProbabilisticPrimitiveGenerator()
{
	initialised = false;
}

void ProbabilisticPrimitiveGenerator::initialise(int depth)
{
	for (int i = 0; i <= depth; i++)
	{
		auto levelDistributionMap = unordered_map<string, double>();

		string fileName = string("levelDist") + to_string(i) + ".txt";
		ifstream file(fileName);
		string line;
		while (getline(file, line))
		{
			auto splitted = StringUtils::Split(line, " ");

			levelDistributionMap[splitted[0]] = stod(splitted[1]);
		}

		levelDistribution.push_back(levelDistributionMap);
	}

	ifstream lcdf("twoNodeDist.txt");

	string line;

	while (getline(lcdf, line))
	{
		auto splitted = StringUtils::Split(line, " ");
		string key = splitted[0] + " " + splitted[1];
		leftChildDistribution[key] = stod(splitted[2]);
	}

	ifstream rcdf("threeNodeDist.txt");

	while (getline(rcdf, line))
	{
		auto splitted = StringUtils::Split(line, " ");

		string key = splitted[0] + " " + splitted[1] + " " + splitted[2];
		rightChildDistribution[key] = stod(splitted[3]);
	}

	initialised = true;
	cout << "Initialised";
}

Tree::PrimitiveP ProbabilisticPrimitiveGenerator::generatePrimitive(int level, string& context)
{
	unordered_map<string, double> adjustedProbabilities;

	for (auto value : levelDistribution[level])
	{
		double probability = 1;
		if(strategy != "contextAware")
		{
			probability = value.second;
		}
		adjustedProbabilities[value.first] = probability;
	}

	if (strategy != "contextFree") {
		unordered_map<string, double> contextBasedProbabilities;
		auto splittedContext = StringUtils::Split(context, " ");
		if (splittedContext.size() == 1)
		{
			for (auto value : leftChildDistribution)
			{
				if (value.first.rfind(context[0],0) == 0)
				{
					auto valueSplit = StringUtils::Split(value.first, " ");
					contextBasedProbabilities[valueSplit[1]] = value.second;
				}
			}
		}
		else if (splittedContext.size() == 2)
		{
			for (auto value : rightChildDistribution)
			{
				if (value.first.rfind(context[0],0) == 0)
				{
					auto valueSplit = StringUtils::Split(value.first, " ");
					contextBasedProbabilities[valueSplit[2]] = value.second;
				}
			}
		}
		else
		{
			cout << "Should not happen";
		}

		for (auto& adjusted_probability : adjustedProbabilities)
		{
			double priority = contextBasedProbabilities[adjusted_probability.first];
			adjusted_probability.second *= priority;
		}
	}

	double totalProbability = 0;
	for (auto& adjusted_probability : adjustedProbabilities)
	{
		totalProbability += adjusted_probability.second;
	}

	for (auto& adjusted_probability : adjustedProbabilities)
	{
		adjusted_probability.second/=totalProbability;
	}


	double probability = state->getRandomizer()->getRandomDouble();
	double probabilitySum = 0.0;
	for (auto element : adjustedProbabilities)
	{
		probabilitySum += element.second;
		if (probability <= probabilitySum)
		{
			return primitiveSet->getPrimitiveByName(element.first);
		}
	}

	return primitiveSet->getRandomPrimitive();
}

Tree::PrimitiveP ProbabilisticPrimitiveGenerator::generateFunction(int level, string& context)
{

	unordered_map<string, double> adjustedProbabilities;

	for (auto value : levelDistribution[level])
	{
		double probability = 0;
		if (primitiveSet->mFunctionSet_.count(value.first) > 0) {
			probability = 1;
			if (strategy != "contextAware")
			{
				probability = value.second;
			}
		}
		
		adjustedProbabilities[value.first] = probability;
	}

	if (strategy != "contextFree" && !context.empty()) {
		unordered_map<string, double> contextBasedProbabilities;
		auto splittedContext = StringUtils::Split(context, " ");
		if (splittedContext.size() == 1)
		{
			for (auto value : leftChildDistribution)
			{
				if (value.first.rfind(context[0], 0) == 0)
				{
					auto valueSplit = StringUtils::Split(value.first, " ");
					if (primitiveSet->mFunctionSet_.count(valueSplit[1]) > 0) {
						contextBasedProbabilities[valueSplit[1]] = value.second;
					}
				}
			}
		}
		else if (splittedContext.size() == 2)
		{
			for (auto value : rightChildDistribution)
			{
				if (value.first.rfind(context[0], 0) == 0)
				{
					auto valueSplit = StringUtils::Split(value.first, " ");
					if (primitiveSet->mFunctionSet_.count(valueSplit[2]) > 0) {
						contextBasedProbabilities[valueSplit[2]] = value.second;
					}
				}
			}
		}
		else
		{
			cout << "Should not happen";
		}

		for (auto& adjusted_probability : adjustedProbabilities)
		{
			double priority = contextBasedProbabilities[adjusted_probability.first];
			adjusted_probability.second *= priority;
		}
	}

	double totalProbability = 0;
	for (auto& adjusted_probability : adjustedProbabilities)
	{
		totalProbability += adjusted_probability.second;
	}

	for (auto& adjusted_probability : adjustedProbabilities)
	{
		adjusted_probability.second /= totalProbability;
	}


	double probability = state->getRandomizer()->getRandomDouble();
	double probabilitySum = 0.0;
	for (auto element : adjustedProbabilities)
	{
		probabilitySum += element.second;
		if (probability <= probabilitySum)
		{
			return primitiveSet->getPrimitiveByName(element.first);
		}
	}

	return primitiveSet->getRandomFunction();

	//double totalFunctionProbability = 0;
	//vector<string> function;
	//vector<double> functionProbabilities;
	//for (auto element : levelDistribution[level])
	//{
	//	if(primitiveSet->mFunctionSet_.count(element.first)==1)
	//	{
	//		function.push_back(element.first);
	//		functionProbabilities.push_back(element.second);
	//		totalFunctionProbability += element.second;
	//	}
	//}

	//for(int i=0; i<functionProbabilities.size(); i++)
	//{
	//	functionProbabilities[i] /= totalFunctionProbability;
	//}
	//
	//double probability = state->getRandomizer()->getRandomDouble();
	//double probabilitySum = 0.0;

	//for (int i =0; i<functionProbabilities.size(); i++)
	//{
	//	probabilitySum += functionProbabilities[i];
	//	if (probability <= probabilitySum)
	//	{
	//		return primitiveSet->getPrimitiveByName(function[i]);
	//	}
	//}

	//
	//return primitiveSet->getRandomFunction();
}

Tree::PrimitiveP ProbabilisticPrimitiveGenerator::generateTerminal(int level, string& context)
{
	unordered_map<string, double> adjustedProbabilities;

	for (auto value : levelDistribution[level])
	{
		double probability = 0;
		if (primitiveSet->mTerminalSet_.count(value.first) > 0) {
			probability = 1;
			if (strategy != "contextAware")
			{
				probability = value.second;
			}
		}

		adjustedProbabilities[value.first] = probability;
	}

	if (strategy != "contextFree" && !context.empty()) {
		unordered_map<string, double> contextBasedProbabilities;
		auto splittedContext = StringUtils::Split(context, " ");
		if (splittedContext.size() == 1)
		{
			for (auto value : leftChildDistribution)
			{
				if (value.first.rfind(context[0], 0) == 0)
				{
					auto valueSplit = StringUtils::Split(value.first, " ");
					if (primitiveSet->mTerminalSet_.count(valueSplit[1]) > 0) {
						contextBasedProbabilities[valueSplit[1]] = value.second;
					}
				}
			}
		}
		else if (splittedContext.size() == 2)
		{
			for (auto value : rightChildDistribution)
			{
				if (value.first.rfind(context[0], 0) == 0)
				{
					auto valueSplit = StringUtils::Split(value.first, " ");
					if (primitiveSet->mTerminalSet_.count(valueSplit[2]) > 0) {
						contextBasedProbabilities[valueSplit[2]] = value.second;
					}
				}
			}
		}
		else
		{
			cout << "Should not happen";
		}

		for (auto& adjusted_probability : adjustedProbabilities)
		{
			double priority = contextBasedProbabilities[adjusted_probability.first];
			adjusted_probability.second *= priority;
		}
	}

	double totalProbability = 0;
	for (auto& adjusted_probability : adjustedProbabilities)
	{
		totalProbability += adjusted_probability.second;
	}

	for (auto& adjusted_probability : adjustedProbabilities)
	{
		adjusted_probability.second /= totalProbability;
	}


	double probability = state->getRandomizer()->getRandomDouble();
	double probabilitySum = 0.0;
	for (auto element : adjustedProbabilities)
	{
		probabilitySum += element.second;
		if (probability <= probabilitySum)
		{
			return primitiveSet->getPrimitiveByName(element.first);
		}
	}

	return primitiveSet->getRandomTerminal();
}
