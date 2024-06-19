#include "DynamicInstanceGenerator.h"

#include <chrono>
#include <random>
#include <unordered_set>

std::string DynamicInstanceGenerator::GenerateInstance()
{
	int maxContainersInYard = GetNumberOfContainers();
	int containersInYard = GetNumberOfContainers();
	std::vector<std::shared_ptr<DummyContainer>> initialYard;
	std::vector<std::shared_ptr<DummyContainer>> yard;

	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_real_distribution eventProbability(0, 1);

	for(int i = 0; i < containersInYard; i++)
	{
		auto container = std::make_shared<DummyContainer>();
		initialYard.push_back(container);
		yard.push_back(container);
	}

	int containersRetrieved = 0;

	std::vector<std::shared_ptr<DummyContainer>> eventContainerIDList;

	while (containersRetrieved <= containersToBeRetrieved)
	{
		if (yard.size() == maxContainersInYard)
		{
			std::uniform_int_distribution<int> dist(0, yard.size() - 1);
			int containerIndex = dist(rng);
			auto container = yard[containerIndex];
			yard.erase(yard.begin() + containerIndex);
			container->id = containersRetrieved;
			eventContainerIDList.push_back(container);
			containersRetrieved++;
		}
		else if (yard.size() == 1)
		{
			auto container = std::shared_ptr<DummyContainer>();
			yard.push_back(container);
			eventContainerIDList.push_back(container);
		}
		else
		{
			if (eventProbability(rng) < arrivalProbability)
			{
				auto container = std::shared_ptr<DummyContainer>();
				yard.push_back(container);
				eventContainerIDList.push_back(container);
			} else
			{
				std::uniform_int_distribution<int> dist(0, yard.size() - 1);
				int containerIndex = dist(rng);
				auto container = yard[containerIndex];
				yard.erase(yard.begin() + containerIndex);
				container->id = containersRetrieved;
				eventContainerIDList.push_back(container);
				containersRetrieved++;
			}
		}
	}

	std::unordered_set<int> arrivedContainers;

	for(auto container:eventContainerIDList)
	{
		
	}

}

std::string DynamicInstanceGenerator::GenerateWeightsForInstance(int minimumWeight, int maximumWeight)
{
	//TODO: not implemented
}

int DynamicInstanceGenerator::GetNumberOfContainers()
{
	return (int)ceil(density * (height * numberOfStacks - height + 1));
}

