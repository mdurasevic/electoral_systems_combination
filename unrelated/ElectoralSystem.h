#pragma once
#include <memory>
#include <vector>

#include "WinnerStrategy.h"

using namespace std;

//abstract class representing the election methods 
class ElectoralSystem
{
public:
	// accepts a vector of vectors in which each vector on the first level represents the priorities calculated for each decision
	// this means that the size of the vector is equal to the number of ensembles, and then in each vector the number of decisions
	// minimise -- denotes that the decision receiving the smallest priority is the best. 
	// return index of the winner
	virtual int elect(vector<vector<double>> &priorities, shared_ptr<WinnerStrategy> winnerStrategy) = 0;
};

