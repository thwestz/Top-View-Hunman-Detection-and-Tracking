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

	//String videoPath = "D:/Senior_Project/Train HOG -2/Video/test/FE99.avi";

	String videoPath = "D:/ProjectCCTV.avi";


	//String videoPath = "D:/20180322/DCS-6010L_20180322141656.avi";

	//String videoPath = "E:/20180326/2.avi";

	//String videoPath = "http://admin:kongkong@10.1.134.159/video1.mjpg";
	//String svmPath = "D:/Senior_Project/head/2042018.xml";
	String svmPath = "D:/my_detector_hn_4.yml";



	BGSubtraction BGSubtractionHeader;

	if (isTrain) {

	}
	else {
		//BGSubtractionHeader.BackgroundSubtraction(videoPath,svmPath);
		BGSubtractionHeader.BackgroundSubtraction(videoPath, svmPath);
	}

	return 0;
}