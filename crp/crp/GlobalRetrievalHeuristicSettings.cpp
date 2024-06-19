#include "GlobalRetrievalHeuristicSettings.h"

GlobalRetrievalHeuristicSettings::GlobalRetrievalHeuristicSettings(std::vector<double> params) : params{ params } {}

void GlobalRetrievalHeuristicSettings::setGRHSettings(std::vector<double> params) {
	this->params = params;
}
std::vector<double> GlobalRetrievalHeuristicSettings::getGRHSettings() {
	return this->params;
}