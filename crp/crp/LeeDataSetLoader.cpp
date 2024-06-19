#include "LeeDataSetLoader.h"
#include "StringUtils.h"
#include <filesystem>
#include <fstream>
#include <unordered_map>

namespace fs = std::filesystem;

DataSet LeeDataSetLoader::Load(string path)
{
	const string delimiter("-");
	const fs::path rootPath{ path };
	
	DataSet ds;

	for (const auto& file : fs::directory_iterator(path))
	{
		ds.AddProblemInstance(LoadInstanceFromFile(file.path().string()));
	}
	return ds;
}

void LeeDataSetLoader::Load(string path, DataSet& dataSet)
{
	for (const auto& file : fs::directory_iterator(path))
	{
		dataSet.AddProblemInstance(LoadInstanceFromFile(file.path().string()));
	}
}


ProblemInstance LeeDataSetLoader::LoadInstanceFromFile(string pathString)
{
	const fs::path path{ pathString };
	const string delimiter(" ");

	std::ifstream fstream{ path };
	string line;

	getline(fstream, line);
	auto split = StringUtils::Split(line, delimiter);
	int bayNumber = stoi(split[1]);
	int stackNumber = stoi(split[2]);
	int maximumStackHeight = stoi(split[3]);
	int containerNumber = stoi(split[4]);

	BayFloor bayFloor(bayNumber, stackNumber);
	std::unordered_map<int, Location> containerLocations;

	int currentStack = 0;
	int currentBay = 0;

	//These instances are by default multibay instances
	bool multibay = true;
	bool duplicate = false;

	
	while (getline(fstream, line))
	{
		split = StringUtils::Split(StringUtils::trim_copy(line), delimiter);

		currentBay = stoi(split[0])-1;
		currentStack = stoi(split[1])-1;
		auto containerLocation = Location(currentBay, currentStack);


		//We must skip the first three elements since they represent some information
		//Also we need to move for two spaces, because this instance has some special format
		for (int i = 3; i < split.size(); i+=2)
		{
			int containerId = stoi(split[i]);
			
			bayFloor.AddContainerToTop(containerLocation, containerId);
			containerLocations.insert(make_pair(containerId, containerLocation));
		}
		
		
		//currentStack = (currentStack+1) % stackNumber ;
	}
	
	string fileName = path.parent_path().filename().string() + "/" + path.filename().string();

	std::vector<int> containerWeights;
	
	return ProblemInstance(fileName, bayNumber, stackNumber, containerNumber, maximumStackHeight, bayFloor, containerLocations, multibay, duplicate, containerWeights);
}
