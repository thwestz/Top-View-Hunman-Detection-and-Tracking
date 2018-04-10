#ifndef TRACKING_H
#define TRACKING_H
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "samples_utility.cpp"
#include "reportTracking.h"
#include "trackStructure.h"
#include "Math.h"
using namespace std;
using namespace cv;
class Tracking {
public:


	MultiTracker adaptMultiTracker(Mat frame, vector<Rect2d> ROIs, MultiTracker currentTrackers,int currentFrame);
	vector<trackStructure> initalID(Mat toTrackimg, MultiTracker currentTrackers, int currentFrame);
	vector<reportTracking> manageReport(vector<trackStructure> currentTrackStruture, vector<reportTracking> pathList);
	void showPath(vector<reportTracking> pathList, Mat pathImg);
};
#endif
