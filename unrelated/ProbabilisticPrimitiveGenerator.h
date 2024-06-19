#pragma once
#include <unordered_map>
#include <ECF/ECF.h>
#include "StringUtils.h"

class ProbabilisticPrimitiveGenerator
{
public:
	vector<unordered_map<string, double>> levelDistribution;
	unordered_map<string, double> leftChildDistribution;
	unordered_map<string, double> rightChildDistribution;
	StateP state;
	Tree::PrimitiveSetP primitiveSet;
	string strategy;
	bool initialised;

	ProbabilisticPrimitiveGenerator();
	void initialise(int depth);
	Tree::PrimitiveP generatePrimitive(int level, string &context);
	Tree::PrimitiveP generateFunction(int level, string& context);
	Tree::PrimitiveP generateTerminal(int level, string& context);

};

