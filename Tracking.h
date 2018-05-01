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
#include <iostream>
#include <fstream>

#include <string>
using namespace std;
using namespace cv;
class Tracking {
public:


	MultiTracker adaptMultiTracker(Mat frame, vector<Rect2d> ROIs, MultiTracker currentTrackers, int currentFrame);
	vector<trackStructure> initalID(MultiTracker currentTrackers, int currentFrame);
	vector<reportTracking> manageReport(vector<trackStructure> currentTrackStruture, vector<reportTracking> pathList);
	void showPath(vector<reportTracking> pathList, Mat pathImg,int fps);
	void showTrack(vector<reportTracking> pathList, vector<trackStructure> currentTrackStruture, Mat trackImg, vector<pair<int, int>>chk_failure_track,int fps, VideoWriter recorder);
	vector<pair<int, int>> cnt_failure_tracking(MultiTracker currentTracker, vector<Rect2d> ROIs, vector<pair<int, int>>chk_failure_track);
	void manageTrack(vector<pair<int, int>> chk_failure_track, vector<trackStructure> currentTrackStructure);
};
#endif
