#pragma once
class ElectionMethodTests
{
public:
	void testElectionMethods();
	bool testPluralityVote();

private:
	bool testPluralityVoteRoundOneWinner();
	bool testPluralityVoteTwoRounds();
	bool testPluralityVoteTwoRoundsNoWinner();
};

