#include "RandomSubDataSet.h"

RandomSubDataSet::RandomSubDataSet(int instanceNumber)
{
	this->instanceNumber = instanceNumber;

}

const ProblemInstance& RandomSubDataSet::GetProblemInstance(int index)
{
	return problemInstances[instanceIndices[index]];
}

int RandomSubDataSet::GetProblemInstanceNumber() const
{
	return instanceNumber;
}

void RandomSubDataSet::SetNextBatch()
{
	std::shuffle(instanceIndices.begin(), instanceIndices.end(), rng);
}

int RandomSubDataSet::GetEntireProblemInstanceSize()
{
	return problemInstances.size();
}

void RandomSubDataSet::AddProblemInstance(ProblemInstance problemInstance)
{
	DataSet::AddProblemInstance(problemInstance);
	instanceIndices.clear();
	for (int i = 0; i < problemInstances.size(); i++)
	{
		instanceIndices.push_back(i);
	}
	rng = std::default_random_engine{};
	std::shuffle(instanceIndices.begin(), instanceIndices.end(), rng);
}

void RandomSubDataSet::UpdateSubsetSize(int subSetSize)
{
	this->instanceNumber = subSetSize;
}

