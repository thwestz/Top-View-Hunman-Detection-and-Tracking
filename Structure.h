#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/objdetect.hpp"
#include <opencv2/tracking.hpp>

#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;
class Structure {
public:
		vector<int> tracking_id(int tracking_id);
		vector<Rect2d> checkROIs(vector<Rect2d> ROIs);
		String status(MultiTracker currentTracker, String status);
};