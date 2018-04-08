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
	void setTrackID(int id);
	int getTrackID();
	void setStatus(int status);
	int getStatus();
	void setROI(Rect2d ROI);
	Rect2d getROI();
	void setFirstFrame(int frame);
	int getFirstFrame();
	void setCenterPoint(Point point);
	Point getCenterPoint();
	void setCntFrame(int cnt_frame);
	int getCntFrame();
private: 
	int track_id;
	int track_status;
	Rect2d track_ROI;
	int firstFrame;
	Point track_centerPoint;
	int trackCntFrame = 0;

};
#endif
