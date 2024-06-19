#include "SumMethod.h"

#include <algorithm>

int SumMethod::elect(vector<vector<double>>& priorities, shared_ptr<WinnerStrategy> winnerStrategy)
{
	vector<double> totalPriorities(priorities[0].size(), 0);

	for (auto total_priority : priorities)
	{
		for(int i = 0; i< total_priority.size(); i++)
		{
			totalPriorities[i] += total_priority[i];
		}
	}

	auto min_it = std::min_element(totalPriorities.begin(), totalPriorities.end());
	auto max_it = std::max_element(totalPriorities.begin(), totalPriorities.end());

	int min_index = std::distance(totalPriorities.begin(), min_it);
	int max_index = std::distance(totalPriorities.begin(), max_it);
	int winnerIndex = winnerStrategy->winner(min_index, max_index);

	return winnerIndex;
}
