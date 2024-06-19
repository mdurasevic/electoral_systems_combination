#pragma once
#include <string>
#include "ProblemInstanceGenerator.h"

class ZhuDuplicateProblemInstanceGenerator : public ProblemInstanceGenerator
{
private:
	int numberOfStacks;
	int containersPerStack;
	int containerNumber;
	double uniquenessFactor;
public:
	ZhuDuplicateProblemInstanceGenerator(int numberOfStacks, int containersPerStack, int containerNumber, double uniquenessFactor);
	std::string GenerateInstance() override;
	std::string GenerateWeightsForInstance(int minimumWeight, int maximumWeight) override;
};

