#pragma once
#include <string>
#include "ProblemInstance.h"
#include "DataSet.h"
#include "DataSetLoader.h"


class ZhuDuplicateDataSetLoader : public DataSetLoader
{
public:
	DataSet Load(std::string path) override;
	void Load(std::string path, DataSet& dataSet) override;
private:
	ProblemInstance LoadInstanceFromFile(std::string path, int maximumStackHeight);
};

