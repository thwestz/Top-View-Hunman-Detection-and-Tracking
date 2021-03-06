#include "BGSubtraction.h"
#include <algorithm>


void BGSubtraction::detectAndTrack(String videoPath, String svmPath)
{

	VideoWriter recorder("D:/Senior_Project/Tracking.avi", CV_FOURCC('X', 'V', 'I', 'D'), 10, Size(800, 600), true);

	vector<Rect2d> testRect;
	Tracking trackingAPI;
	HOG_SVM HOG_SVMHeader;
	MultiTracker currentTrack;
	vector<trackStructure> currentTrackStruture;
	vector<reportTracking> pathList;
	vector<trackStructure> trackManage;
	Mat bg_im, bg_im_gray, image_gray, diff_im, bw, first_frame, pathImg;
	Mat image, detectImg, toTrackimg, trackImg, originalImg, diff_new, bw_new, cleaned_im;
	Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(7, 7));
	Mat SE(5, 5, CV_8U, Scalar(1));
	double th = 40;

	int cnt_firstFrame = 0, counter = 0, cnt_id = 0, fps = 0, initId = 0;

	vector<pair<int, int>> chk_failure_track;
	VideoCapture stream1(videoPath);
	fps = stream1.get(CV_CAP_PROP_FPS);
	printf_s("FPS: %d \n", fps);
	stream1.read(bw);

	while (1)
	{
		vector<Rect2d> reversePoint, newPointList, newPointListToTrack;
		vector<vector<Point>> contours;

		if (!(stream1.read(image))) {
			break;
		}

		stream1 >> image;

		detectImg = image.clone();
		trackImg = image.clone();
		originalImg = image.clone();
		resize(detectImg, toTrackimg, Size(detectImg.cols, detectImg.rows));

		if (cnt_firstFrame == 0)
		{
			first_frame = image;
			bg_im = first_frame;
			pathImg = first_frame.clone();
			pathImg = bg_im.clone();
			cvtColor(bg_im, bg_im_gray, COLOR_BGR2GRAY);

		}

		/// Pre-Image Processing///
		cvtColor(image, image_gray, COLOR_BGR2GRAY);
		absdiff(bg_im_gray, image_gray, diff_im);
		threshold(diff_im, bw, th, 255, THRESH_BINARY);
		dilate(bw, bw, structuringElement7x7);

		cvtColor(diff_im, diff_new, COLOR_GRAY2BGR);
		cvtColor(bw, bw_new, COLOR_GRAY2BGR);

		/// Find Interest Region
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

			int tlx_o = 0, tly_o = 0, brx_o = 0, bry_o = 0;

			/// Controll Size of Region
			if (areas[i] > 1000)
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
				Mat crop = originalImg(pointCrop);

				/// HOG-SVM Prediction
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
						newPointListToTrack.push_back(Rect2d(Point(tlx, tly), Point(brx, bry)));
					}


					for (int z = 0; z < newPointList.size(); z++) {
						rectangle(detectImg, newPointList[z], Scalar(255, 0, 0), 2);
					}

				}

			}

		}
		//imshow("Image", image);
		//imshow("Diff", diff_im);
		//imshow("bw", bw);
		imshow("HOG-SVM Detector", detectImg);
		/// Tracking Algorithm

		currentTrack = trackingAPI.adaptMultiTracker(toTrackimg, newPointListToTrack, currentTrack, cnt_firstFrame);
		currentTrackStruture = trackingAPI.initalID(currentTrack, cnt_firstFrame);

		chk_failure_track = trackingAPI.cnt_failure_tracking(currentTrack, newPointListToTrack, chk_failure_track);
		/// Manage Report
		pathList = trackingAPI.manageReport(currentTrackStruture, pathList);

		trackingAPI.showTrack(pathList, currentTrackStruture, trackImg, chk_failure_track, fps);


		cnt_firstFrame++;
		newPointList.clear();
		reversePoint.clear();
		if (waitKey(20) >= 0)
			break;
	}
	trackingAPI.showPath(pathList, pathImg, fps);
	destroyAllWindows();
}