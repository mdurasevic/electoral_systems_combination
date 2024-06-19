#pragma once

#include "ZhuDataSetLoader.h"
#include "StringUtils.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Location.h"

namespace fs = std::filesystem;

DataSet ZhuDataSetLoader::Load(string path)
{
	const string delimiter("-");
	const fs::path rootPath{ path };
	DataSet ds;
	
	for(const auto& file : fs::directory_iterator(rootPath))
	{
		string directoryName = file.path().filename().string();
		auto splitDirectory = StringUtils::Split(directoryName, delimiter);
		int maximumStackHeight = stoi(splitDirectory[0]);
		
		for (const auto& instanceFile : fs::directory_iterator(file.path()))
		{
			if (instanceFile.is_regular_file()) {
				ds.AddProblemInstance(LoadInstanceFromFile(instanceFile.path().string(), maximumStackHeight));
			}
		}
	}
	
	return ds;
}

void ZhuDataSetLoader::Load(string path, DataSet& dataSet)
{
	const string delimiter("-");
	const fs::path rootPath{ path };
	for (const auto& file : fs::directory_iterator(rootPath))
	{
		string directoryName = file.path().filename().string();
		auto splitDirectory = StringUtils::Split(directoryName, delimiter);
		int maximumStackHeight = stoi(splitDirectory[0]);

		for (const auto& instanceFile : fs::directory_iterator(file.path()))
		{
			if (instanceFile.is_regular_file()) {
				dataSet.AddProblemInstance(LoadInstanceFromFile(instanceFile.path().string(), maximumStackHeight));
			}
		}
	}
}

ProblemInstance ZhuDataSetLoader::LoadInstanceFromFile(string pathString, int maximumStackHeight)
{
	const fs::path path{ pathString };
	const string delimiter(" ");
	
	std::ifstream fstream{ path };
	string line;

	do {
		getline(fstream, line);
	} while (line.starts_with("#"));
	auto split = StringUtils::Split(line, delimiter);
	//In these instances we only have one bay!
	int bayNumber = 1;
	int stackNumber = stoi(split[0]);
	int containerNumber = stoi(split[1]);

	BayFloor bayFloor(bayNumber, stackNumber);
	std::unordered_map<int, Location> containerLocations;

	bool multibay = false;
	bool duplicate = false;
	int maximumContainerId = 0;

	
	int currentStack = 0;
	int currentBay = 0;
	while (getline(fstream, line))
	{
		split = StringUtils::Split(StringUtils::trim_copy(line), delimiter);

		auto containerLocation = Location(currentBay, currentStack);
		
		
		//We must skip the first since it denotes only the number of containers in this stack
		for(int i = 1; i<split.size(); i++)
		{
			int containerId = stoi(split[i]);


			if (maximumContainerId < containerId)
			{
				maximumContainerId = containerId;
			}
			
			bayFloor.AddContainerToTop(containerLocation, containerId);
			containerLocations.insert(make_pair(containerId, containerLocation));
		}
		currentStack++;
	}

	if (containerNumber != maximumContainerId)
	{
		duplicate = true;
	}
	
	string fileName = path.parent_path().filename().string()+ "/" + path.filename().string();

	const fs::path weightPath{ path.parent_path().generic_string() + "/" + "weights" + "/" + path.filename().generic_string() };
	vector<int> containerWeights;
	if (fs::exists(weightPath))
	{
		ifstream weightFile{ weightPath };
		while (getline(weightFile, line))
		{
			auto split = StringUtils::Split(line, " ");
			containerWeights.push_back(stoi(split[1]));
		}
	}

	return ProblemInstance(fileName, bayNumber, stackNumber, containerNumber, maximumStackHeight, bayFloor, containerLocations, multibay, duplicate, containerWeights);
}
