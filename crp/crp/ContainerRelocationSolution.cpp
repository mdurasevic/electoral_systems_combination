#include "ContainerRelocationSolution.h"

ContainerRelocationSolution::ContainerRelocationSolution(const std::string& instanceName, int numberOfMoves,
	double totalExecutionTime, double total_energy_consumed, const std::vector<ContainerMove>& containerMoves)
{
	this->instanceName = instanceName;
	this->numberOfMoves = numberOfMoves;
	this->totalExecutionTime = totalExecutionTime;
	this->containerMoves = containerMoves;
	this->totaEnergyConsumption = total_energy_consumed;
}

std::string ContainerRelocationSolution::getInstanceName()
{
	return instanceName;
}

int ContainerRelocationSolution::GetNumberOfMoves()
{
	return numberOfMoves;
}

double ContainerRelocationSolution::GetTotalExecutionTime()
{
	return  totalExecutionTime;
}

double ContainerRelocationSolution::GetTotalEnergyConsumption()
{
	return totaEnergyConsumption;
}

std::vector<ContainerMove> ContainerRelocationSolution::getContainerMoves()
{
	return containerMoves;
}
