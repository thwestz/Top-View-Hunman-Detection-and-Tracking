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

vector<Rect2d> reportTracking::getPathRect()
{
	return pathRect;
}

void reportTracking::addPathRect(Rect2d rect)
{
	pathRect.push_back(rect);
}

Rect2d reportTracking::getOnePath(int index)
{
	return pathRect[index];
}

