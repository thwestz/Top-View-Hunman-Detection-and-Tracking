#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace cv;
Point point1, point2;
int drag = 0;
Rect rect; /* bounding box */
Mat img, roiImg; /* roiImg - the part of the image in the bounding box */
int select_flag = 0;
string filePath;
ofstream outputfile;


void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{

	if (event == CV_EVENT_LBUTTONDOWN && !drag)
	{
		/* left button clicked. ROI selection begins */
		point1 = Point(x, y);
		drag = 1;
	}

	if (event == CV_EVENT_MOUSEMOVE && drag)
	{
		/* mouse dragged. ROI being selected */
		Mat img1 = img.clone();
		point2 = Point(x, y);
		rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 3, 8, 0);
		imshow("image", img1);
	}

	if (event == CV_EVENT_LBUTTONUP && drag)
	{
		point2 = Point(x, y);
		rect = Rect(point1.x, point1.y, x - point1.x, y - point1.y);

		drag = 0;
		roiImg = img(rect);
		destroyWindow("ROI");
	}

	if (event == CV_EVENT_LBUTTONUP)
	{

		if (point1.x == x) {
			cout << "Error :: ROI is low size" << "\n";
		}
		else {
			/* ROI selected */
			drag = 0;
	
			imshow("Regoin of Interesting", roiImg);
			namedWindow("Regoin of Interesting", WINDOW_AUTOSIZE);
			cout << (point1.x) << "," << (point1.y) << "," << (x) << "," << (y) << "\n";
			outputfile << (point1.x) << "," << (point1.y) << "," << (x) << "," << (y) << "\n";
		}

	}
}

int main(int argc, char** argv)
{
	vector<String> input_img;
	glob("D:/Senior_Project/head/pic", input_img);
	for (int i = 0; i < input_img.size(); i++) {
		//if fail to read the image
		cout << input_img[i] << "\n";
		if (i > input_img.size())
		{
			cout << "Error loading the image" << endl;
			return -1;
		}
		img = imread(input_img[i]);
		imshow("image", img);


		std::string::size_type start;
		std::string::size_type end;
		start = input_img[i].find("\\");
		end = input_img[i].find(".jpg");
		int surPosition = end - start - 1;
		filePath = input_img[i].substr(start + 1 , surPosition);

		outputfile.open("D:/Senior_Project/Train HOG -2/Image/exam/point/"+ filePath + ".txt", std::ios_base::app);
		if (!outputfile.is_open()) {
			
			std::ofstream newFile("D:/Senior_Project/Train HOG -2/Image/exam/point/" + filePath + ".txt");
			newFile.close();
			outputfile.open("D:/Senior_Project/Train HOG -2/Image/exam/point/" + filePath + ".txt", std::ios_base::app);
		}

		//set the callback function for any mouse event
		setMouseCallback("image", CallBackFunc, NULL);

		waitKey(0);

		destroyAllWindows();
		outputfile.close();
	}
	return 0;

}