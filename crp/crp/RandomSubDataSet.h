#pragma once
#include "DataSet.h"
#include <algorithm> 
#include <random>

class RandomSubDataSet :
    public DataSet
{
private:
	int instanceNumber;
	std::vector<int> instanceIndices;
	std::default_random_engine rng;
	bool initialised;
	
public:
	RandomSubDataSet(int instanceNumber);
	const ProblemInstance& GetProblemInstance(int index) override;
	int GetProblemInstanceNumber() const override;
	void SetNextBatch();
	int GetEntireProblemInstanceSize();
	void UpdateSubsetSize(int subSetSize);
	void AddProblemInstance(ProblemInstance problemInstance) override;

};

