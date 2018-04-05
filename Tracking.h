#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "trackStructure.h"
using namespace std;
using namespace cv;
class Tracking {
public:


	MultiTracker tracking_API(Mat frame, vector<Rect2d> ROIs, MultiTracker currentTrackers,vector<trackStructure> trackManage,int currentFrame);


};
