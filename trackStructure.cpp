#include "trackStructure.h"


void trackStructure::setTrackID(size_t id)
{
	track_id = id;
}

size_t trackStructure::getTrackID()
{
	return track_id;
}

void trackStructure::setStatus(int status)
{
	track_status = status;
}

int trackStructure::getStatus()
{
	return  track_status;
}

void trackStructure::setROI(Rect2d ROI)
{
	track_ROI = ROI;
}

Rect2d trackStructure::getROI()
{
	return track_ROI;
}

void trackStructure::setFirstFrame(int frame)
{
	frame = frame;
}

int trackStructure::getFirstFrame()
{
	return firstFrame;
}

void trackStructure::addCenterPoint(Point point)
{
	centerPoint.push_back(point);
}

Point trackStructure::getCenterPoint(int index)
{
	return centerPoint[index];
}

void trackStructure::removeCenterPoint(int index)
{
	centerPoint.erase(centerPoint.begin() + index);
}

void trackStructure::setCntFrame(int cnt_frame) 
{
	trackCntFrame = cnt_frame;
}

int trackStructure::getCntFrame()
{
	return trackCntFrame;
}

