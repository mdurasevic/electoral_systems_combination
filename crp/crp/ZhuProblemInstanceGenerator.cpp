#include "ZhuProblemInstanceGenerator.h"
#include <deque>
#include <random>
#include <sstream>
#include  <algorithm>
#include <ctime>
#include <unordered_set>

ZhuProblemInstanceGenerator::ZhuProblemInstanceGenerator(int numberOfStacks, int containersPerStack, int containerNumber)
{
	this->numberOfStacks = numberOfStacks;
	this->containersPerStack = containersPerStack;
	this->containerNumber = containerNumber;
	srand(time(NULL));
} 

std::string ZhuProblemInstanceGenerator::GenerateInstance()
{
	int relocationPlaces = numberOfStacks * containersPerStack - containerNumber;
	std::deque<int> containers;
	std::vector<int> uniqueContainers;

	for (int i = 1; i <= containerNumber; i++)
	{
		containers.push_back(i);
	}


	auto rng = std::default_random_engine{};
	std::shuffle(containers.begin(), containers.end(), rng);

	std::stringstream problemInstance;

	problemInstance << numberOfStacks << " " << containerNumber << "\n";
	std::vector<std::string> stackStrings;
	std::vector<int> stackSize;
	std::unordered_set<int> freeStacks;

	for(int i =0; i<numberOfStacks; i++)
	{
		stackStrings.push_back("");
		stackSize.push_back(0);
		freeStacks.insert(i);
	}

		while (!containers.empty())
		{
			int containerId = containers.front();
			int position = *std::next(freeStacks.begin(), rand() % freeStacks.size());
			int height = stackSize[position];
			containers.pop_front();
			if ((containersPerStack - relocationPlaces - containerId) <= height)
			{
				stackStrings[position] += (std::string(" ") + std::to_string(containerId));
				stackSize[position]++;
				if (stackSize[position] == containersPerStack)
				{
					freeStacks.erase(position);
				}
			}
			else
			{
				bool foundStack = false;
				for (auto element : freeStacks)
				{
					if ((containersPerStack - relocationPlaces - containerId) <= stackSize[position])
					{
						stackStrings[position] += (std::string(" ") + std::to_string(containerId));
						stackSize[position]++;
						if (stackSize[position] == containersPerStack)
						{
							freeStacks.erase(position);
						}
						foundStack = true;
					}
				}
				if (!foundStack)
				{
					containers.push_back(containerId);
				}
			}
		}
	

	for(int stack = 0; stack < numberOfStacks; stack++)
	{
		problemInstance << stackSize[stack] << stackStrings[stack] <<"\n";
	}
	
	problemInstance << "\n";

	return problemInstance.str();
}

std::string ZhuProblemInstanceGenerator::GenerateWeightsForInstance(int minimumWeight, int maximumWeight)
{
	std::stringstream instanceWeights;

	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<int> dist(minimumWeight, maximumWeight);

	for (int i = 1; i <= containerNumber; i++)
	{
		instanceWeights << std::to_string(i) << " " << dist(rng) << "\n";
	}
	return instanceWeights.str();
}
