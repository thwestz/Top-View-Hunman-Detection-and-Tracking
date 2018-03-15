#include <stdio.h>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
using namespace cv;
using namespace std;
using namespace cv::ml;

vector<Point> pointList, pointList_neg;
void jittering(int numJit, Point pY, Point pJ, Mat &image,bool jit_x, int &count);

void to_Point(string sPoint) {

	int n, x = 0, y = 0, k = 0, j = 0;
	string tmp;

	n = sPoint.find(",");
	tmp = sPoint.substr(n + 1);
	x = stoi(sPoint.substr(0, n));

	n = tmp.find(",");
	tmp = tmp.substr(0);
	y = stoi(tmp.substr(0, n));
	tmp = tmp.substr(n + 1);

	n = tmp.find(",");
	k = stoi(tmp.substr(0, n));
	tmp = tmp.substr(n + 1);

	j = stoi(tmp);

	pointList.push_back(Point(x, y));
	pointList.push_back(Point(k, j));

}

int main()
{
	vector<String> pos_files, neg_files, point_files;
	Mat image;
	RNG rng;
	rng(getTickCount());

	glob("D:/Project_Oriental/Test train HOG -1/FishEye/dataset_image", pos_files);
	glob("D:/Project_Oriental/Test train HOG -1/FishEye/dataset_point", point_files);

	int k;
	k = 0;
	Size win_size(64, 128);
	Point startPoint;
	Point endPoint;
	int j = 0;
	int count = 0;
	for (int i = 0; i < point_files.size(); i++)
	{
		cout << pos_files[i] << "\n\n";
		cout << point_files[i] << "\n\n";
		string line;
		ifstream myfile(point_files[i]);
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				to_Point(line);
			}
			myfile.close();

		}
		image = imread(pos_files[i], IMREAD_COLOR);

		cout << pointList.size() << "\n\n";
		for (int y = 0; y < pointList.size(); y++) {
			j = y + 1;
			if (j < pointList.size()) {
				startPoint = Point(pointList[y]);
				endPoint = Point(pointList[j]);

				
				Rect roi(startPoint, endPoint);
		
				Mat crop = image(roi);
				
				//cout << crop.rows << "    " << crop.cols << "\n\n";
				//imshow("Original", image);
				//imshow("Crop", crop);
				//clog << startPoint << endPoint << endl;
				resize(crop, crop,Size(64,64));
				imwrite("D:/Project_Oriental/Test train HOG -1/FishEye/Crop/images_" + to_string(count) + ".jpg", crop);

				bool jitter = false;
				if (jitter) {
					// jit grand x
					jittering(10,pointList[y], pointList[j],image,true,count);
					jittering(20, pointList[y], pointList[j], image, true, count);
					jittering(-10, pointList[y], pointList[j], image, true,count);
					jittering(-20, pointList[y], pointList[j], image, true,count);

					// jit grand y
					jittering(10, pointList[y], pointList[j], image,false,count);
					jittering(20, pointList[y], pointList[j], image,false,count);
					jittering(-10, pointList[y], pointList[j], image, false,count);
					jittering(-20, pointList[y], pointList[j], image, false,count);
				}
				y++;
				count++;
			}
		}
		pointList.clear();
		j = 0;
	}

}

	void jittering(int numJit ,Point pY , Point pJ , Mat &image , bool jit_x ,int &count) {

		if (jit_x) {
			Point startPoint = Point(pY.x + numJit, pY.y);
			Point endPoint = Point(pJ.x + numJit, pJ.y);
			
			if (startPoint.x < 0) {
				startPoint.x = 0;
			}

			if (startPoint.x > image.cols - 1) {
				startPoint.x = image.cols;
			}

			if (endPoint.x < 0) {
				endPoint.x = 0;
			}

			if (endPoint.x > image.cols - 1) {
				endPoint.x = image.cols;
			}

			Rect jROI(startPoint, endPoint);
			Mat jitImg = image(jROI);
			imwrite("D:/Senior_Project/head/crop_floor18_2/images_J_" + to_string(numJit) + "_"+ to_string(count) + ".jpg", jitImg);
		}
		else {
			Point startPoint = Point(pY.x, pY.y + numJit);
			Point endPoint = Point(pJ.x, pJ.y + numJit);


			if (startPoint.y < 0) {
				startPoint.y = 0;
			}

			if (startPoint.y > image.rows - 1) {
				startPoint.y = image.rows;
			}

			if (endPoint.y < 0) {
				endPoint.y = 0;
			}

			if (endPoint.y > image.rows - 1) {
				endPoint.y = image.rows;
			}


			Rect jROI(startPoint, endPoint);
			Mat jitImg = image(jROI);
			
			imwrite("D:/Senior_Project/head/crop_floor18_2/images_J_" + to_string(numJit) + "_" + to_string(count) + ".jpg", jitImg);

		}
	}

