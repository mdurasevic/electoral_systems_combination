#pragma once
#include "ElectoralSystem.h"

enum class MedianRule{TypicalJudgement, UsualJudgement, CentralJudgement, MajorityJudgement};

class JudgementMethod :
    public ElectoralSystem
{
public:
	JudgementMethod(int maximumRating=4, int minimumRating=0, MedianRule medianRule = MedianRule::TypicalJudgement);
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy) override;

private:
	int minimumRating;
	int maximumRating;
	MedianRule medianRule;
};

