#pragma once
#include <string>

class ProblemInstanceGenerator
{
public:
	virtual std::string GenerateInstance() = 0;
	virtual std::string GenerateWeightsForInstance(int minimumWeight=1, int maximumWeight=30) = 0;
};

