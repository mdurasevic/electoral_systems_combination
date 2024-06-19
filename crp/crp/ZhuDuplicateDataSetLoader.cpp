#include "ZhuDuplicateDataSetLoader.h"

#include "StringUtils.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Location.h"

namespace fs = std::filesystem;

DataSet ZhuDuplicateDataSetLoader::Load(string path)
{
	const string delimiter("-");
	const fs::path rootPath{ path };
	DataSet ds;

	for (const auto& file : fs::directory_iterator(rootPath))
	{
		string directoryName = file.path().filename().string();
		auto splitDirectory = StringUtils::Split(directoryName, delimiter);
		int maximumStackHeight = stoi(splitDirectory[0]);

		for (const auto& instanceFile : fs::directory_iterator(file.path()))
		{
			ds.AddProblemInstance(LoadInstanceFromFile(instanceFile.path().string(), maximumStackHeight));
		}
	}

	return ds;
}

void ZhuDuplicateDataSetLoader::Load(string path, DataSet& dataSet)
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
			dataSet.AddProblemInstance(LoadInstanceFromFile(instanceFile.path().string(), maximumStackHeight));
		}
	}
}

ProblemInstance ZhuDuplicateDataSetLoader::LoadInstanceFromFile(string pathString, int maximumStackHeight)
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
//	std::unordered_map<int, Location> containerLocations;
	std::unordered_map<int, std::set<Location>> containerLocations;


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
		for (int i = 1; i < split.size(); i++)
		{
			int containerId = stoi(split[i]);


			if (maximumContainerId < containerId)
			{
				maximumContainerId = containerId;
			}

			bayFloor.AddContainerToTop(containerLocation, containerId);
			if (!containerLocations.contains(containerId))
			{
				containerLocations.insert(std::pair<int, std::set<Location>>(containerId, std::set<Location>()));
			}
			containerLocations.at(containerId).insert(containerLocation);
		}
		currentStack++;
	}

	duplicate = true;

	string fileName = path.parent_path().filename().string() + "/" + path.filename().string();

	return ProblemInstance(fileName, bayNumber, stackNumber, containerNumber, maximumStackHeight, bayFloor, containerLocations, multibay, duplicate);

}
