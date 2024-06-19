#pragma once
#include "Location.h"

class ContainerMove
{
private:
	Location source;
	Location destination;
public:
	ContainerMove(Location source, Location destination);
	ContainerMove(Location source);
	Location GetSourceLocation();
	Location GetDestinationLocation();
	bool IsRelocation();
	bool IsRemoval();
};

