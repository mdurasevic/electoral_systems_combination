#pragma once
#include "ElectoralSystem.h"

enum class BordaCountStrategy { StandardBorda, BinaryNumberSystem, Nauru, Eurovision };

class BordaCount : public ElectoralSystem  
{
public:
	BordaCount(BordaCountStrategy gradingStrategy);
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;
private:
	BordaCountStrategy gradingStrategy;
	vector<int> eurovisionScores = { 12,10,8,7,6,5,4,3,2,1 };
};

