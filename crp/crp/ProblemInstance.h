#pragma once
#include "Location.h"
#include <map>
#include <string>
#include "ContainerYard.h"
#include <unordered_map>

class ProblemInstance
{
private:
	std::string instanceName;
	int bayNumber;
	int containerNumber;
	int stackNumber;
	int maximumStackHeight;

	bool multibay;
	bool duplicate;

	BayFloor bayFloor;
	std::unordered_map<int, Location> containerLocation;
	std::unordered_map<int, std::set<Location>> containerLocations;

	std::vector<int> containerCount;
	std::vector<int> containerWeights;
	
public:
	ProblemInstance(std::string instanceName, int bayNumber, int stackNumber, int containerNumber, int maximumStackHeight, BayFloor bayFloor, std::unordered_map<int, std::set<Location>> containerLocations, bool multibay, bool duplicate);
	ProblemInstance(std::string instanceName, int bayNumber, int stackNumber, int containerNumber, int maximumStackHeight, BayFloor bayFloor, std::unordered_map<int, Location> containerLocation, bool multibay, bool duplicate, std::vector<int> containerWeights);
	ContainerYard CreateContainerYard(double gantrySpeed, double trolleySpeed, double gantryAcceleration, double pickUpTime, int maxHeight = -1, double uncertainty = 1.0);
	std::string GetInstanceName();

private:
	void InitialiseContainerCount();
};

