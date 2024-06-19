#include "ContainerMove.h"

ContainerMove::ContainerMove(Location source, Location destination) : source(source), destination(destination)
{
}

ContainerMove::ContainerMove(Location source) : source(source), destination(-1,-1)
{
}


Location ContainerMove::GetSourceLocation()
{
	return source;
}

Location ContainerMove::GetDestinationLocation()
{
	return destination;
}


bool ContainerMove::IsRelocation()
{
	return destination.GetStack()!=-1;
}

bool ContainerMove::IsRemoval()
{
	return !IsRelocation();
}


