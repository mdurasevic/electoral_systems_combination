#pragma once
#include "ProblemInstanceGenerator.h"

class ZhuProblemInstanceGenerator : public ProblemInstanceGenerator
{
private:
	int numberOfStacks;
	int containersPerStack;
	int containerNumber;
public:
	ZhuProblemInstanceGenerator(int numberOfStacks, int containersPerStack, int containerNumber);
	std::string GenerateInstance() override;
	std::string GenerateWeightsForInstance(int minimumWeight=1, int maximumWeight=30) override;
};

