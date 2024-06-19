#include "ElectionMethodTests.h"

#include <iostream>

#include "LowestPriorityStrategy.h"
#include "PluralityVote.h"

void ElectionMethodTests::testElectionMethods()
{
	testPluralityVote();
}

bool ElectionMethodTests::testPluralityVote()
{
	bool allPass = true;
	allPass = testPluralityVoteRoundOneWinner();
	allPass = testPluralityVoteTwoRounds();
	if(allPass)
	{
		cout << "All tests for the plurality vote method passes correctly";
		return true;
	}
	return false;
}

bool ElectionMethodTests::testPluralityVoteRoundOneWinner()
{
	vector<vector<double>> priorities{
		{1,2,3},
		{2,3,1},
		{1,3,2}
	};
	
	shared_ptr<WinnerStrategy> winnerStrategy = make_shared<LowestPriorityStrategy>();
	PluralityVote voting;
	int winner = voting.elect(priorities, winnerStrategy);
	if (winner == 0)
	{
		return true;
	} else
	{
		cout << "Basic plurality vote not working correctly"<<endl;
		return false;
	}

}

bool ElectionMethodTests::testPluralityVoteTwoRounds()
{
	vector<vector<double>> priorities{
	{1,3,2},
	{2,3,1},
	{1,3,2},
	{2,3,1},
	{3,1,2}
	};

	shared_ptr<WinnerStrategy> winnerStrategy = make_shared<LowestPriorityStrategy>();
	PluralityVote voting;
	int winner = voting.elect(priorities, winnerStrategy);
	if (winner == 2)
	{
		return true;
	}
	else
	{
		cout << "Basic plurality vote not working correctly in two stages" << endl;;
		return false;
	}
}

bool ElectionMethodTests::testPluralityVoteTwoRoundsNoWinner()
{
	vector<vector<double>> priorities{
{1,3,2},
{2,3,1},
{1,3,2},
{2,3,1}
	};

	shared_ptr<WinnerStrategy> winnerStrategy = make_shared<LowestPriorityStrategy>();
	PluralityVote voting;
	int winner = voting.elect(priorities, winnerStrategy);
	if (winner == 0)
	{
		return true;
	}
	else
	{
		cout << "Basic plurality vote not working correctly in two stages" << endl;;
		return false;
	}
}
