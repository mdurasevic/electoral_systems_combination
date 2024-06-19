#include "SolutionStorage.h"

#include <filesystem>

namespace fs = std::filesystem;

void SolutionStorage::Save(string rootPathString, std::shared_ptr<ContainerRelocationSolution> solution)
{
	const fs::path rootPath{ rootPathString };
	fs::path filePath = rootPath / fs::path{ solution->getInstanceName() + ".txt" };
	ofstream file;
	file.open(filePath);

	file << solution->getInstanceName() << endl;
	file << solution->GetNumberOfMoves() << endl;
	file << solution->GetTotalExecutionTime() << endl;
	file << solution->GetTotalEnergyConsumption() << endl;

	for (auto containerMove : solution->getContainerMoves())
	{
		file << containerMove.GetSourceLocation().GetBay();
		file << " ";
		file << containerMove.GetSourceLocation().GetStack();
		file << " ";
		file << containerMove.GetDestinationLocation().GetBay();
		file << " ";
		file << containerMove.GetDestinationLocation().GetStack();
		file << endl;
	}
	
	file.close();
}

std::shared_ptr<ContainerRelocationSolution> SolutionStorage::Load(string filePath)
{
	string line;
	ifstream input(filePath);
	
		string instanceName;
		int numberOfMoves;
		double totalExecutionTime;
		double totalEnergyConsumption;

		input >> instanceName;
		input >> numberOfMoves;
		input >> totalExecutionTime;
		input >> totalEnergyConsumption;

		vector<ContainerMove> containerMoves;

		int sourceBay;
		int sourceStack;
		int destinationBay;
		int destinationStack;
	
		while(input >> sourceBay >> sourceStack >> destinationBay >> destinationStack)
		{				
				Location source(sourceBay, sourceStack);
				Location destination(destinationBay, destinationStack);		
				containerMoves.push_back(ContainerMove(source, destination));
		}

		return shared_ptr<ContainerRelocationSolution>(new ContainerRelocationSolution(instanceName, numberOfMoves, totalExecutionTime, totalEnergyConsumption,  containerMoves));
}
