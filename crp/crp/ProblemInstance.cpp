#include "ProblemInstance.h"
#include <memory>
#include <string>

ProblemInstance::ProblemInstance(std::string instanceName, int bayNumber, int stackNumber, int containerNumber, int maximumStackHeight,
	BayFloor bayFloor,
	std::unordered_map<int, std::set<Location>> containerLocations, bool multibay, bool duplicate):
bayFloor(std::move(bayFloor))
{
	this->instanceName = instanceName;
	this->bayNumber = bayNumber;
	this->stackNumber = stackNumber;
	this->containerLocations = std::move(containerLocations);
	this->containerNumber = containerNumber;
	this->maximumStackHeight = maximumStackHeight;
	this->multibay = multibay;
	this->duplicate = duplicate;
	InitialiseContainerCount();
}

ProblemInstance::ProblemInstance(std::string instanceName, int bayNumber, int stackNumber, int containerNumber,
	int maximumStackHeight, BayFloor bayFloor, std::unordered_map<int, Location> containerLocation, bool multibay,
	bool duplicate, std::vector<int> containerWeights) :
	bayFloor(std::move(bayFloor))
{
	this->instanceName = instanceName;
	this->bayNumber = bayNumber;
	this->stackNumber = stackNumber;
	this->containerLocation = std::move(containerLocation);
	this->containerNumber = containerNumber;
	this->maximumStackHeight = maximumStackHeight;
	this->multibay = multibay;
	this->duplicate = duplicate;
	this->containerWeights = std::move(containerWeights);
	InitialiseContainerCount();
}

ContainerYard ProblemInstance::CreateContainerYard(double gantrySpeed, double trolleySpeed, double gantryAcceleration, double pickUpTime, int maxHeight, double unceartainty)
{
	if(duplicate)
	{
		return ContainerYard(instanceName, bayFloor, containerLocations, containerCount, bayNumber, containerNumber, stackNumber, maximumStackHeight, gantrySpeed, trolleySpeed, gantryAcceleration, pickUpTime, multibay, duplicate, unceartainty, false, false);
	}
	else
	{
		return ContainerYard(instanceName, bayFloor, containerLocation, containerCount, bayNumber, containerNumber, stackNumber, maximumStackHeight, gantrySpeed, trolleySpeed, gantryAcceleration, pickUpTime, multibay, duplicate,containerWeights, unceartainty, false, false);
	}
}

std::string ProblemInstance::GetInstanceName()
{
	return  this->instanceName;
}

void ProblemInstance::InitialiseContainerCount()
{
	if (!duplicate)
	{
		for (int i = 0; i < containerNumber; i++)
		{
			containerCount.push_back(1);
		}
	} else
	{
		containerCount.resize(containerLocations.size()+1);
		std::fill(containerCount.begin(), containerCount.end(), 0);
		
		for(int i = 0; i<bayNumber; i++)
		{
			for(int j=0; j<stackNumber; j++)
			{
				Location location(i, j);
				for(int k=0; k<bayFloor.GetStackHeight(location); k++)
				{
					int container = bayFloor.GetContainerAtHeight(location, k);
					containerCount[container]++;
				}
			}
		}
	}
}
 