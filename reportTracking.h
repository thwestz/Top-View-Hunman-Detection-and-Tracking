#ifndef REPORT_TRACKING_H
#define REPORT_TRACKING_H

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"

#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;
class reportTracking
{
public:

	int getID();
	void setID(int id);

	vector<Point> getPath();
	void addPath(Point path);


private :

	int rt_id;
	vector<Point> pathList;
};
#endif
