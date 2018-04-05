#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include "BGSubtraction.h"
#include "HOG_SVM.h"
using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	bool isTrain = false;

	//String videoPath = "D:/Senior_Project/Train HOG -2/Video/test/FE99.avi";

	//String videoPath = "D:/ProjectCCTV.avi";


	//String videoPath = "D:/20180322/DCS-6010L_20180322141656.avi";

	String videoPath = "E:/20180326/35.avi";

	//String videoPath = "http://admin:kongkong@10.1.134.159/video1.mjpg";
	String svmPath = "D:/Senior_Project/head/2042018.xml";

	//String svmPath = "D:/my_detector_hn_4.yml";



	BGSubtraction BGSubtractionHeader;
	HOG_SVM HOGSVM;


	if (isTrain) {
		String pos_dir = "";
		String neg_dir = "";
		String test_dir = "";
		String obj_det_filename = "";
		HOGSVM.TrainHOG_SVM(pos_dir,neg_dir, test_dir, obj_det_filename);
		BGSubtractionHeader.BackgroundSubtraction(videoPath, svmPath);
	}
	else {


	BGSubtractionHeader.BackgroundSubtraction(videoPath, svmPath);
	}

	return 0;
}