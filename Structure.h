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
	void setTracking_id(int element,size_t total_rois,int id);
	vector<int>	 getTracking_id();
	void setStatus(String status);
	String getStatus();
private :
	vector<int> tracking_id;
	String tracking_status;
};