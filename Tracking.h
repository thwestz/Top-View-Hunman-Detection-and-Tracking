#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ctime>

using namespace std;
using namespace cv;
class Tracking {
public:

	MultiTracker tracking_API(Mat frame, vector<Rect> ROIs, MultiTracker currentTrackers);
};
