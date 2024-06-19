#include "LeeProblemInstanceGenerator.h"
#include <ctime>
#include <deque>
#include <chrono>
#include <sstream>
#include <unordered_set>

struct pair_hash {
	inline std::size_t operator()(const std::pair<int, int>& v) const {
		return v.first * 31 + v.second;
	}
};

LeeProblemInstanceGenerator::LeeProblemInstanceGenerator(int numberOfBays, int numberOfStacks, int height, int numberOfContainers)
{
	this->numberOfBays = numberOfBays;
	this->numberOfStacks = numberOfStacks;
	this->height = height;
	this->numberOfCOntainers = numberOfContainers;
	srand(time(NULL));
}
//RAndom generation
//std::string LeeProblemInstanceGenerator::GenerateInstance()
//{
//	std::deque<int> containers;
//	for (int i = 1; i <= numberOfCOntainers; i++)
//	{
//		containers.push_back(i);
//	}
//
//	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//	auto rng = std::default_random_engine{ seed };
//	std::shuffle(containers.begin(), containers.end(), rng);
//
//	std::stringstream problemInstance;
//
//	std::vector<std::vector<std::string>> stackStrings;
//	std::vector<std::vector<int>> stackSize;
//	std::unordered_set<std::pair<int, int>, pair_hash> freeStacks;
//
//	for (int b = 0; b < numberOfBays; b++)
//	{
//		stackStrings.push_back(std::vector<std::string>());
//		stackSize.push_back(std::vector<int>());
//
//		for (int i = 0; i < numberOfStacks; i++)
//		{
//			stackStrings[b].push_back("");
//			stackSize[b].push_back(0);
//			freeStacks.insert(std::make_pair(b, i));
//		}
//	}
//
//	while (!containers.empty())
//	{
//		int containerId = containers.front();
//		std::pair position = *std::next(freeStacks.begin(), rand() % freeStacks.size());
//		containers.pop_front();
//
//		stackStrings[position.first][position.second] += (std::string(" ") + std::to_string(containerId)) + (std::string(" ") + std::to_string(containerId));
//		stackSize[position.first][position.second]++;
//		if (stackSize[position.first][position.second] == height)
//		{
//			freeStacks.erase(position);
//		}
//	}
//
//	for (int b = 0; b < numberOfBays; b++){
//		for (int stack = 0; stack < numberOfStacks; stack++)
//		{
//			problemInstance << " " << b+1 << " "<<stack+1<< " "<< stackSize[b][stack] << stackStrings[b][stack] << "\n";
//		}
//	}
//	//problemInstance << "\n";
//	return problemInstance.str();
//}


// generate uniform
//std::string LeeProblemInstanceGenerator::GenerateInstance()
//{
//	std::deque<int> containers;
//	for (int i = 1; i <= numberOfCOntainers; i++)
//	{
//		containers.push_back(i);
//	}
//
//	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//	auto rng = std::default_random_engine{ seed };
//	std::shuffle(containers.begin(), containers.end(), rng);
//
//	std::stringstream problemInstance;
//
//	std::vector<std::vector<std::string>> stackStrings;
//	std::vector<std::vector<int>> stackSize;
//	std::unordered_set<std::pair<int, int>, pair_hash> freeStacks;
//
//	for (int b = 0; b < numberOfBays; b++)
//	{
//		stackStrings.push_back(std::vector<std::string>());
//		stackSize.push_back(std::vector<int>());
//
//		for (int i = 0; i < numberOfStacks; i++)
//		{
//			stackStrings[b].push_back("");
//			stackSize[b].push_back(0);
//			freeStacks.insert(std::make_pair(b, i));
//		}
//	}
//
//	int positionIndex = 0;
//	while (!containers.empty())
//	{
//		int containerId = containers.front();
//		std::pair position = *std::next(freeStacks.begin(), positionIndex);
//		positionIndex = (positionIndex + 1) % freeStacks.size();
//		containers.pop_front();
//
//		stackStrings[position.first][position.second] += (std::string(" ") + std::to_string(containerId)) + (std::string(" ") + std::to_string(containerId));
//		stackSize[position.first][position.second]++;
//		if (stackSize[position.first][position.second] == height)
//		{
//			freeStacks.erase(position);
//		}
//	}
//
//	for (int b = 0; b < numberOfBays; b++) {
//		for (int stack = 0; stack < numberOfStacks; stack++)
//		{
//			problemInstance << " " << b + 1 << " " << stack + 1 << " " << stackSize[b][stack] << stackStrings[b][stack] << "\n";
//		}
//	}
//	//problemInstance << "\n";
//	return problemInstance.str();
//}

// full first
//std::string LeeProblemInstanceGenerator::GenerateInstance()
//{
//	std::deque<int> containers;
//	for (int i = 1; i <= numberOfCOntainers; i++)
//	{
//		containers.push_back(i);
//	}
//
//	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//	auto rng = std::default_random_engine{ seed };
//	std::shuffle(containers.begin(), containers.end(), rng);
//
//	std::stringstream problemInstance;
//
//	std::vector<std::vector<std::string>> stackStrings;
//	std::vector<std::vector<int>> stackSize;
//	std::unordered_set<std::pair<int, int>, pair_hash> freeStacks;
//
//	for (int b = 0; b < numberOfBays; b++)
//	{
//		stackStrings.push_back(std::vector<std::string>());
//		stackSize.push_back(std::vector<int>());
//
//		for (int i = 0; i < numberOfStacks; i++)
//		{
//			stackStrings[b].push_back("");
//			stackSize[b].push_back(0);
//			freeStacks.insert(std::make_pair(b, i));
//		}
//	}
//
//	while (!containers.empty())
//	{
//		int containerId = containers.front();
//		std::pair position = *std::next(freeStacks.begin(), 0);
//		containers.pop_front();
//
//		stackStrings[position.first][position.second] += (std::string(" ") + std::to_string(containerId)) + (std::string(" ") + std::to_string(containerId));
//		stackSize[position.first][position.second]++;
//		if (stackSize[position.first][position.second] == height)
//		{
//			freeStacks.erase(position);
//		}
//	}
//
//	for (int b = 0; b < numberOfBays; b++) {
//		for (int stack = 0; stack < numberOfStacks; stack++)
//		{
//			problemInstance << " " << b + 1 << " " << stack + 1 << " " << stackSize[b][stack] << stackStrings[b][stack] << "\n";
//		}
//	}
//	//problemInstance << "\n";
//	return problemInstance.str();
//}

//descending
std::string LeeProblemInstanceGenerator::GenerateInstance()
{
	std::deque<int> containers;
	for (int i = 1; i <= numberOfCOntainers; i++)
	{
		containers.push_back(i);
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	auto rng = std::default_random_engine{ seed };
	std::shuffle(containers.begin(), containers.end(), rng);

	std::stringstream problemInstance;

	std::vector<std::vector<std::string>> stackStrings;
	std::vector<std::vector<int>> stackSize;
	std::unordered_set<std::pair<int, int>, pair_hash> freeStacks;

	for (int b = 0; b < numberOfBays; b++)
	{
		stackStrings.push_back(std::vector<std::string>());
		stackSize.push_back(std::vector<int>());

		for (int i = 0; i < numberOfStacks; i++)
		{
			stackStrings[b].push_back("");
			stackSize[b].push_back(0);
			freeStacks.insert(std::make_pair(b, i));
		}
	}

	int corrector = numberOfBays*numberOfStacks*0.1;
	int counter = 0;
	
	while (!containers.empty())
	{
		int containerId = containers.front();
		std::pair position = *std::next(freeStacks.begin(), 0);
		containers.pop_front();

		stackStrings[position.first][position.second] += (std::string(" ") + std::to_string(containerId)) + (std::string(" ") + std::to_string(containerId));
		stackSize[position.first][position.second]++;
		if (stackSize[position.first][position.second] == (int)(height-counter/corrector))
		{
			freeStacks.erase(position);
			counter++;
		}
	}

	

	for (int b = 0; b < numberOfBays; b++) {
		for (int stack = 0; stack < numberOfStacks; stack++)
		{
			problemInstance << " " << b + 1 << " " << stack + 1 << " " << stackSize[b][stack] << stackStrings[b][stack] << "\n";
		}
	}
	//problemInstance << "\n";
	return problemInstance.str();
}

std::string LeeProblemInstanceGenerator::GenerateWeightsForInstance(int minimumWeight, int maximumWeight)
{
	return "";
}
