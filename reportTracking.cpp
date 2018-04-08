#include "reportTracking.h"

int reportTracking::getID()
{
	return rt_id;
}

void reportTracking::setID(int id)
{
	rt_id = id;
}


vector<Point> reportTracking::getPath()
{
	return pathList;
}

void reportTracking::addPath(Point path)
{
	pathList.push_back(path);
}

