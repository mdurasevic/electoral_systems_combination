#pragma once
#include  "DataSetLoader.h"

class ZhuDataSetLoader : public DataSetLoader
{
public:
	DataSet Load(string path) override;
	void Load(string path, DataSet& dataSet) override;
private:
	ProblemInstance LoadInstanceFromFile(string path, int maximumStackHeight);
};