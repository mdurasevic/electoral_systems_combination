#pragma once
#include "ContainerMove.h"
#include <vector>
#include <string>

class ContainerRelocationSolution
{
private:
	std::string instanceName;
	int numberOfMoves;
	double totalExecutionTime;
	double totaEnergyConsumption;
	std::vector<ContainerMove> containerMoves;

public:
	ContainerRelocationSolution(const std::string& instance_name, int number_of_moves, double total_execution_time, double tota_energy_consumed,
		const std::vector<ContainerMove>& moves);

	std::string getInstanceName();
	int GetNumberOfMoves();
	double GetTotalExecutionTime();
	double GetTotalEnergyConsumption();
	std::vector<ContainerMove> getContainerMoves();
};

