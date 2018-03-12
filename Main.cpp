#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include "BGSubtraction.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	bool isTrain = false;

	String videoPath = "D:/Senior_Project/Train HOG -2/Video/train/4.avi";

	String svmPath = "D:/Senior_Project/head/my_detector_hn_3.yml";

	BGSubtraction BGSubtractionHeader;

	if (isTrain) {

	}
	else {
		//BGSubtractionHeader.BackgroundSubtraction(videoPath,svmPath);
		BGSubtractionHeader.BackgroundSubtraction(videoPath,svmPath);
	}

	return 0;
}