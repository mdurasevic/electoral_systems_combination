#pragma once
#include<vector>

enum GRHSettings {
	alpha, //Importance of minimizing hoisting
	beta, //Importance of minimizing lowering
	gamma, //Importance of minimizing trolleying
	P1,  //Importance of minimizing hoisting, lowering, and trolleying of heavy (versus light) containers
	delta, //Importance of minimizing rehandling
	P2, //Importance of minimizing rehandling of heavy (versus light) containers
	epsilon,  //Importance of delaying rehandling
	P3,  //Importance of delaying rehandling of heavy (versus light) containers
	eta, //Importance of tightness
	theta, //Importance of moving containers closer to the truck lane
	P4, //Importance of moving heavy (versus light) containers closer to truck lane
	mi //Importance of keeping stack heights low
};

class GlobalRetrievalHeuristicSettings
{
private:
	std::vector<double> params;

public:
	GlobalRetrievalHeuristicSettings() :params{} {};
	GlobalRetrievalHeuristicSettings(std::vector<double> params);
	void setGRHSettings(std::vector<double> params);
	std::vector<double> getGRHSettings();

};
