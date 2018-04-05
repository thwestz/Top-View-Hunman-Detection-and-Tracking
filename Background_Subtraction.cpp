#include "BGSubtraction.h"
#include "HOG_SVM.h"
#include "Tracking.h"
#include "trackStructure.h"

void BGSubtraction::BackgroundSubtraction(String videoPath, String svmPath)
{
	vector < Ptr<Tracker>> algorithms;
	vector<Rect2d> objects;
	MultiTracker currentTrack;
	int counter = 0;
	Mat bg_im, bg_im_gray, image_gray, diff_im, bw, first_frame;
	Mat showResultDetection;
	Tracking tracking;
	double th = 50;
	VideoCapture stream1(videoPath);
	stream1.read(bw);
	stream1.set(CAP_PROP_POS_FRAMES, 100);
	int i = 1;
	int k = 0;
	Mat path;
	vector<trackStructure> trackManage;

	while (1)
	{
		Mat image, test;

		if (!(stream1.read(image))) {
			break;
		}

		stream1 >> image;
		//imwrite("D:/Senior_Project/Train HOG -2/Video/train/bg_" + to_string(k) + ".jpg", image);
		//resize(image, image, Size(image.cols * 1.75, image.rows * 1.75));
		//undistort(test, image, camera_matrix, distCoeffs);*/
		Mat newPoint = image.clone();
		Mat toTrackimg;
		resize(newPoint, toTrackimg, Size(newPoint.cols, newPoint.rows));

		Mat o_image = image.clone();


		if (i == 1)
		{
			first_frame = image;
			bg_im = first_frame;
			//bg_im = imread("E:/New folder/images_4_20.jpg");
			path = bg_im.clone();
			//resize(bg_im, bg_im, Size(bg_im.cols * 1.75, bg_im.rows * 1.75));
			cvtColor(bg_im, bg_im_gray, COLOR_BGR2GRAY);
			//GaussianBlur(bg_im_gray, bg_im_gray, Size(3,3), 0, 0, BORDER_DEFAULT);
		}
		cvtColor(image, image_gray, COLOR_BGR2GRAY);
		//GaussianBlur(image_gray, image_gray, Size(3, 3), 0, 0, BORDER_DEFAULT);
		absdiff(bg_im_gray, image_gray, diff_im);
		threshold(diff_im, bw, th, 255, THRESH_BINARY);
		cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(7, 7));
		cv::dilate(bw, bw, structuringElement7x7);
		cv::dilate(bw, bw, structuringElement7x7);
		Mat SE(5, 5, CV_8U, Scalar(1));

		Mat cleaned_im;
		morphologyEx(bw, cleaned_im, MORPH_OPEN, SE);
		Mat diff_new, bw_new;
		cvtColor(diff_im, diff_new, COLOR_GRAY2BGR);
		cvtColor(bw, bw_new, COLOR_GRAY2BGR);
		HOG_SVM HOG_SVMHeader;
		vector<Rect2d> reversePoint, newPointList, newPointListToTrack;
		std::vector<std::vector<Point>> contours;

		findContours(bw, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		vector<double> areas(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{

			areas[i] = contourArea(contours[i]);



			Rect bb = boundingRect(contours[i]);
			int sx = bb.tl().x;
			int sy = bb.tl().y;
			int ex = bb.br().x - 1;
			int ey = bb.br().y - 1;
			int cx = (sx + ex) / 2;
			int cy = (sy + ey) / 2;

			int tlx_o = 0;
			int tly_o = 0;
			int brx_o = 0;
			int bry_o = 0;

			if (areas[i] > 1500)
			{

				tlx_o = bb.tl().x - 20;
				tly_o = bb.tl().y - 20;
				brx_o = bb.br().x + 20;
				bry_o = bb.br().y + 20;
				rectangle(image, bb, Scalar(0, 255, 0), 2);
				if (tlx_o <= 0) {
					tlx_o = 0;
				}


				if (tly_o <= 0) {
					tly_o = 0;
				}

				if (brx_o >= image.cols) {
					brx_o = image.cols;
				}

				if (bry_o >= image.rows) {
					bry_o = image.rows;
				}

				Rect pointCrop(Point(tlx_o, tly_o), Point(brx_o, bry_o));
				Mat crop = o_image(pointCrop);
				vector<Rect> predicted = HOG_SVMHeader.test_trained_detector(svmPath, crop, true, counter);
				if (predicted.size() <= 0) {
					continue;
				}
				else {
					for (int x = 0; x < predicted.size(); x++) {
						int tlx = pointCrop.tl().x + predicted[x].tl().x;
						int tly = pointCrop.tl().y + predicted[x].tl().y;

						int brx = tlx + (predicted[x].br().x - predicted[x].tl().x);
						int bry = tly + (predicted[x].br().y - predicted[x].tl().y);

						newPointList.push_back(Rect2d(Point(tlx, tly), Point(brx, bry)));
						newPointListToTrack.push_back(Rect2d(Point(tlx, tly ), Point(brx, bry)));
					}

					for (int z = 0; z < newPointList.size(); z++) {
						rectangle(newPoint, newPointList[z], Scalar(255, 0, 0), 2);
						for (int t = 0; t < newPointListToTrack.size(); t++) {
						}
					}

				}

			}

		}
		imshow("Image", image);
		//imshow("Diff", diff_im);
		imshow("bw", bw);
		imshow("newPoint", newPoint);
		//imwrite("E:/evaluate/39/image2042018_" + to_string(i) + ".jpg",newPoint);
		currentTrack = tracking.tracking_API(toTrackimg, newPointListToTrack, currentTrack,trackManage,i);
		//for (int m = 0; m < currentTrack.getObjects().size(); m++) {
		//	Point center_of_rect = (currentTrack.getObjects()[m].br() + currentTrack.getObjects()[m].tl())*0.5;
		//	circle(path, center_of_rect, 1, Scalar(0, 0, 255),2);
		//}
		//imshow("path", path);
		i++;
		/*if (i == 40) {
			imshow("path", path);
			waitKey(0);
		}*/
		newPointList.clear();
		reversePoint.clear();
		if (waitKey(20) >= 0)
			break;
	}
	destroyAllWindows();
	
}