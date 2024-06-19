#pragma once
#include <random>

#include "ProblemInstanceGenerator.h"

class DynamicInstanceGenerator :
    public ProblemInstanceGenerator
{
	int numberOfStacks;
	int height;
	double density;
	int containersToBeRetrieved;
	double arrivalProbability;
	std::default_random_engine rng;
	std::vector<std::string> eventList;
public:
	std::string GenerateInstance() override;
	std::string GenerateWeightsForInstance(int minimumWeight, int maximumWeight) override;
	int GetNumberOfContainers();
};

class DummyContainer
{
public:
	int id;

};