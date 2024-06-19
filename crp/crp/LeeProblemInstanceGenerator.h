#pragma once
#include "ProblemInstanceGenerator.h"
#include <random>

class LeeProblemInstanceGenerator :
    public ProblemInstanceGenerator
{
private:
	int numberOfStacks;
	int numberOfBays;
	int height;
	int numberOfCOntainers;
	std::default_random_engine rng;

public:
	LeeProblemInstanceGenerator(int numberOfBays, int numberOfStacks, int height, int numberOfContainers);
	std::string GenerateInstance() override;
	std::string GenerateWeightsForInstance(int minimumWeight, int maximumWeight) override;
};

