#pragma once
#include "ElectoralSystem.h"


/*
 * Winner is the one who receives the highest number of votes.
 * In case of a tie:
 * 1) winner is the first one to receive the highest number of votes
 * 2) A second round is carried out to determine the winner but only between those that had the highest vote count in the first round
 */
class PluralityVote : public ElectoralSystem
{
public:
	PluralityVote(bool useSecondRound = true);
	int elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winerStrategy) override;
private:
	bool useSecondRound;
};

