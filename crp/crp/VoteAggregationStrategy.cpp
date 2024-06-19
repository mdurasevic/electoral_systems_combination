#include "VoteAggregationStrategy.h"

std::vector<double> VoteAggregationStrategy::aggregate(std::vector<std::vector<double>>& priorities)
{
	std::vector<double> votes;

	votes.resize(priorities[0].size(), 0);
	
	for(int i =0; i<priorities.size(); i++)
	{
		int index = 0;
		double bestPriority = priorities[i][0];

		for(int j=1; j<priorities[i].size();j++)
		{
			if(priorities[i][j]<bestPriority)
			{
				bestPriority = priorities[i][j];
				index = j;
			}
		}

		votes[index]++;
	}

	return votes;
}
