#ifndef TRACKSTRUCTURE_H
#define TRACKSTRUCTURE_H

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"

#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;

class trackStructure
{
public:
	void setTrackID(size_t id);
	size_t getTrackID();
	void setStatus(int status);
	int getStatus();
	void setROI(Rect2d ROI);
	Rect2d getROI();
	void setFirstFrame(int frame);
	int getFirstFrame();
	void addCenterPoint(Point point);
	Point getCenterPoint(int index);
	void removeCenterPoint(int index);

private: 
	size_t track_id;
	int track_status;
	Rect2d ROI;
	int firstFrame;
	vector<Point> centerPoint;

};
#endif
