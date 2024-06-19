#include "SequentialSubDataSet.h"

SequentialSubDataSet::SequentialSubDataSet(int instanceNumber)
{
	this->instanceNumber = instanceNumber;
	offset = 0;
}

const ProblemInstance& SequentialSubDataSet::GetProblemInstance(int index)
{
	return problemInstances[(offset + index) % problemInstances.size()];
}

int SequentialSubDataSet::GetProblemInstanceNumber() const
{
	return instanceNumber;
}

void SequentialSubDataSet::SetNextBatch()
{
	offset += instanceNumber;
	offset %= problemInstances.size();
}

int SequentialSubDataSet::GetEntireProblemInstanceSize()
{
	return problemInstances.size();
}

void SequentialSubDataSet::UpdateSubsetSize(int subSetSize)
{
	this->instanceNumber = subSetSize;
	offset = 0;
}
