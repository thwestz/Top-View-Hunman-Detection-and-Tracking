#ifndef BGSUBTRACTION_H
#define BGSUBTRACTION_H
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>

#include "HOG_SVM.h"
#include "Tracking.h"
#include "trackStructure.h"
#include "reportTracking.h"
#include <algorithm>
using namespace cv;
using namespace std;

class BGSubtraction {
public:


	void detectAndTrack(String videoPath, String svmPath);
};
#endif
