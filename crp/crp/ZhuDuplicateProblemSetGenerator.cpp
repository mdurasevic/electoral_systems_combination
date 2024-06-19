#include "ZhuDuplicateProblemSetGenerator.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "ZhuDuplicateProblemInstanceGenerator.h"

void ZhuDuplicateProblemSetGenerator::Generate()
{
	std::vector<int> stacks = { 6, 7, 8 , 9, 10 };
	std::vector<int> containersPerStacks = { 3, 4, 5, 6, 7 };

	int instancesPerParameter = 10;

	std::string folder = "zhuduptrain\\alpha=0.2";

	std::filesystem::create_directories(folder);

	double uniquenessFactor = 0.2;
	
	for (auto stack : stacks)
	{
		for (auto containersPerStack : containersPerStacks)
		{
			int minContainerNumber = (stack - 1) * containersPerStack;
			int maxContainerNumber = stack * containersPerStack - 1;
			for (int containers = minContainerNumber; containers <= maxContainerNumber; containers++) {
				ZhuDuplicateProblemInstanceGenerator generator(stack, containersPerStack, containers, uniquenessFactor);

				std::stringstream directoryName;
				directoryName << folder << '\\' << containersPerStack << "-" << stack << "-" << containers;

				std::filesystem::create_directories(directoryName.str());
				for (int instance = 0; instance < instancesPerParameter; instance++)
				{

					std::string	instanceString = generator.GenerateInstance();
					std::stringstream fileName;
					fileName << folder << '\\' << containersPerStack << "-" << stack << "-" << containers << "\\" << instance << ".txt";
					std::ofstream out(fileName.str());
					out << instanceString;
					out.close();
				}
			}
		}
	}
}
