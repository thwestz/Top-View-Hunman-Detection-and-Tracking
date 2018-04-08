#include "trackStructure.h"


void trackStructure::setTrackID(int id)
{
	track_id = id;
}

int trackStructure::getTrackID()
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

void trackStructure::setCenterPoint(Point point)
{
	track_centerPoint =point;
}

Point trackStructure::getCenterPoint()
{
	return track_centerPoint;
}


void trackStructure::setCntFrame(int cnt_frame) 
{
	trackCntFrame = cnt_frame;
}

int trackStructure::getCntFrame()
{
	return trackCntFrame;
}

