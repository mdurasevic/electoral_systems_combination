#include "ZhuProblemSetGenerator.h"
#include "ZhuProblemInstanceGenerator.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

void ZhuProblemSetGenerator::Generate(std::string outputLocation)
{
	std::vector<int> stacks = { 6, 7, 8 , 9, 10 };
	std::vector<int> containersPerStacks = { 3, 4, 5, 6, 7 };

	int instancesPerParameter = 10;

	//std::string folder = "zhuvalidation";
	
	std::filesystem::create_directories(outputLocation);

	for (auto stack : stacks)
	{
		for (auto containersPerStack : containersPerStacks)
		{
			int minContainerNumber = (stack-1)*containersPerStack;
			int maxContainerNumber = stack*containersPerStack -1;
			for (int containers = minContainerNumber; containers <= maxContainerNumber; containers++) {
				ZhuProblemInstanceGenerator generator(stack, containersPerStack, containers);

				std::stringstream directoryName;
				directoryName<< outputLocation << '\\' << containersPerStack << "-" << stack << "-" << containers;
				
				std::filesystem::create_directories(directoryName.str());
				for (int instance = 0; instance < instancesPerParameter; instance++)
				{
					
					std::string	instanceString = generator.GenerateInstance();
					std::stringstream fileName;
					fileName << outputLocation << '\\' << containersPerStack<<"-"<<stack<<"-"<<containers<<"\\" << instance << ".txt";
					std::ofstream out(fileName.str());
					out << instanceString;
					out.close();
				}
			}
		}
	}
}

void ZhuProblemSetGenerator::GenerateInstanceWeights(std::string outputLocation)
{
	std::vector<int> stacks = { 6, 7, 8 , 9, 10 };
	std::vector<int> containersPerStacks = { 3, 4, 5, 6, 7 };

	int instancesPerParameter = 10;

	std::filesystem::create_directories(outputLocation);

	for (auto stack : stacks)
	{
		for (auto containersPerStack : containersPerStacks)
		{
			int minContainerNumber = (stack - 1) * containersPerStack;
			int maxContainerNumber = stack * containersPerStack - 1;
			for (int containers = minContainerNumber; containers <= maxContainerNumber; containers++) {
				ZhuProblemInstanceGenerator generator(stack, containersPerStack, containers);

				std::stringstream directoryName;
				directoryName << outputLocation << '\\' << containersPerStack << "-" << stack << "-" << containers;

				std::filesystem::create_directories(directoryName.str());
				std::filesystem::create_directories(directoryName.str() + "\\weights");
				for (int instance = 0; instance < instancesPerParameter; instance++)
				{

					std::string	instanceString = generator.GenerateWeightsForInstance();
					std::stringstream fileName;
					std::filesystem::create_directories(directoryName.str());
					fileName << outputLocation << "\\" << containersPerStack << "-" << stack << "-" << containers << "\\" << "weights" << "\\"  << instance << ".txt";
					std::ofstream out(fileName.str());
					out << instanceString;
					out.close();
				}
			}
		}
	}
}
