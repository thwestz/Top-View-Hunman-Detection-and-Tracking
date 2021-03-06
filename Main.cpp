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


	String videoPath = "D:/Senior_Project/Data/t1.avi";


	//String videoPath = "http://admin:kongkong@10.1.134.159/video1.mjpg";

	String svmPath = "D:/Senior_Project/Model/18042018-1C001.xml";




	BGSubtraction BGSubtractionHeader;
	HOG_SVM HOGSVM;


	if (isTrain) {
		String pos_dir = "D:/Senior_Project/Dataset/Train";
		String neg_dir = "D:/Project_Oriental/Test train HOG -1/Negative Resouce";
		String test_dir = "D:/Senior_Project/head/test";
		String obj_det_filename = "D:/Senior_Project/head/06042018-1.xml";
		String videofilename = "";
		HOGSVM.train_detector(pos_dir,neg_dir, test_dir, obj_det_filename, videofilename);
		BGSubtractionHeader.detectAndTrack(videoPath, svmPath);
	}
	else {

	BGSubtractionHeader.detectAndTrack(videoPath, svmPath);
	}

	return 0;
}
