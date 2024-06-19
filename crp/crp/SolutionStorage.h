#pragma once
#include <memory>
#include "ContainerRelocationSolution.h"
#include <iostream>
#include <fstream>

using namespace std;


class SolutionStorage
{
public:
	void Save(string rootPath, std::shared_ptr<ContainerRelocationSolution> solution);
	std::shared_ptr<ContainerRelocationSolution> Load(string filePath);
};

