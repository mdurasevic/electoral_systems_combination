#pragma once
#include "DataSet.h"
class SequentialSubDataSet :
    public DataSet
{
private:
	int instanceNumber;
	int offset;

public:
	SequentialSubDataSet(int instanceNumber);
	const ProblemInstance& GetProblemInstance(int index) override;
	int GetProblemInstanceNumber() const override;
	void SetNextBatch();
	int GetEntireProblemInstanceSize();
	void UpdateSubsetSize(int subSetSize);
};

