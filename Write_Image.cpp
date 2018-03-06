#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;
int k = 0;
int delay = 10;
void writeImage(String filePath) {
	Mat frame, saveFrame;

	VideoCapture capture(filePath);

	if (!capture.isOpened())
		return;

	for (; ; )
	{
		delay--;
		capture >> frame;
		if (frame.empty())
			break;
		if (delay == 0) {
			k++;
			resize(frame, saveFrame, Size(frame.cols, frame.rows));
			imwrite("D:/Senior_Project/Train HOG -2/Image/fortrain/images_4_" + to_string(k) + ".jpg", saveFrame);
			cout << "images" << to_string(k) << ".jpg" << "\n";
			delay = 10;
		}
	}
}

int main(int argc, char** argv)
{
	vector<String> fileName;
	glob("D:/Senior_Project/Train HOG -2/Video/train", fileName);
	for (int i = 0; i < fileName.size(); i++) {
		cout << "file : " << fileName[i] << "\n";
		writeImage(fileName[i]);
	}

	return 0;
}