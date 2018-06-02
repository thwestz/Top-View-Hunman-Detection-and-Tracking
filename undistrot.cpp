#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>
#include "stdafx.h"

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;



int main(int argc, char** argv) {

	Mat camera_matrix = Mat(3, 3, CV_32FC1);
	Mat newCamMat = Mat(3, 3, CV_32FC1);
	Mat distCoeffs;
	FileStorage fs("D:/Project/calibrate/out_camera_data_40.xml", FileStorage::READ);
	fs["camera_matrix"] >> camera_matrix;
	fs["distortion_coefficients"] >> distCoeffs;
	fs.release();

	cout << camera_matrix << "CAM" << "   " << distCoeffs << "Coef";

	VideoCapture cap("D:/Project/Video/FE10.mp4"); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	for (;;)
	{
		Mat frame ,view,dst;
		cap >> frame; // get a new frame from camera

		resize(frame, dst, Size(800, 600));
		undistort(dst, view, camera_matrix, distCoeffs);

		imshow("original", dst);
		imshow("undistort", view);
		if (waitKey(30) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}