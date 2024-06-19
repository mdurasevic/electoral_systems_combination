#pragma once
#include "ElectoralSystem.h"
class ScoreVoting :
    public ElectoralSystem
{
public:
	ScoreVoting(int maxScore = 10, int minScore = 0);
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;
private:
	int maxScore;
	int minScore;
};

