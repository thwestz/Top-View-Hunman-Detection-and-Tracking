#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
using namespace cv;
using namespace std;

class BGSubtraction {
public:


	void BackgroundSubtraction(String videoPath,String svmPath);
	void BackgroundSubtraction_old(String videoPath, String svmPath);
	Mat templeteMatching(vector<Mat> obj , Mat templ);
};
