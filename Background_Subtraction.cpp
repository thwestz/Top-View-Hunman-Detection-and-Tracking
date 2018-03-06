#include "BGSubtraction.h"
#include "Blob.h"
#include "HOG_SVM.h"
const  Scalar SCALAR_BLACK =  Scalar(0.0, 0.0, 0.0);
const  Scalar SCALAR_WHITE =  Scalar(255.0, 255.0, 255.0);
const  Scalar SCALAR_BLUE =  Scalar(255.0, 0.0, 0.0);
const  Scalar SCALAR_GREEN =  Scalar(0.0, 200.0, 0.0);
const  Scalar SCALAR_RED =  Scalar(0.0, 0.0, 255.0);

void BGSubtraction::BackgroundSubtraction(String videoPath,String svmPath) {

	 VideoCapture capVideo;

	 Mat imgFrame1;
	 Mat imgFrame2;
	 vector<Mat> ROI;
	capVideo.open(videoPath);

	if (!capVideo.isOpened()) {                                               
		 cout << "\nerror reading video file" <<  endl <<  endl;   
	}

	if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
		 cout << "\nerror: video file must have at least two frames";
	}

	capVideo.read(imgFrame1);
	capVideo.read(imgFrame2);

	char chCheckForEscKey = 0;

	while (capVideo.isOpened() && chCheckForEscKey != 27) {

		 vector<Blob> blobs;

		 Mat imgFrame1Copy = imgFrame1.clone();
		 Mat imgFrame2Copy = imgFrame2.clone();
		 Mat imgFrame3Copy = imgFrame2.clone();

		 Mat imgDifference;
		 Mat imgThresh;

		 cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
		 cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

		 GaussianBlur(imgFrame1Copy, imgFrame1Copy,  Size(5, 5), 0);
		 GaussianBlur(imgFrame2Copy, imgFrame2Copy,  Size(5, 5), 0);

		 absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

		 threshold(imgDifference, imgThresh, 18, 255.0, CV_THRESH_BINARY);

		 //imshow("imgThresh", imgThresh);

		 Mat structuringElement3x3 =  getStructuringElement( MORPH_RECT,  Size(3, 3));
		 Mat structuringElement5x5 =  getStructuringElement( MORPH_RECT,  Size(5, 5));
		 Mat structuringElement7x7 =  getStructuringElement( MORPH_RECT,  Size(7, 7));
		 Mat structuringElement9x9 =  getStructuringElement( MORPH_RECT,  Size(9, 9));

		 dilate(imgThresh, imgThresh, structuringElement5x5);
		 dilate(imgThresh, imgThresh, structuringElement5x5);
		 erode(imgThresh, imgThresh, structuringElement5x5);

		 Mat imgThreshCopy = imgThresh.clone();

		 vector< vector< Point> > contours;

		 findContours(imgThreshCopy, contours,  RETR_EXTERNAL,  CHAIN_APPROX_SIMPLE);

		 Mat imgContours(imgThresh.size(), CV_8UC3, SCALAR_BLACK);

		 drawContours(imgContours, contours, -1, SCALAR_WHITE, -1);

		//imshow("imgContours", imgContours);

		 vector< vector< Point> > convexHulls(contours.size());

		for (unsigned int i = 0; i < contours.size(); i++) {
			 convexHull(contours[i], convexHulls[i]);
		}

		for (auto &convexHull : convexHulls) {
			Blob possibleBlob(convexHull);

			if (possibleBlob.boundingRect.area() > 100 &&
				possibleBlob.dblAspectRatio >= 0.2 &&
				possibleBlob.dblAspectRatio <= 1.2 &&
				possibleBlob.boundingRect.width > 15 &&
				possibleBlob.boundingRect.height > 20 &&
				possibleBlob.dblDiagonalSize > 30.0) {
				blobs.push_back(possibleBlob);
			}
		}

		 Mat imgConvexHulls(imgThresh.size(), CV_8UC3, SCALAR_BLACK);

		convexHulls.clear();

		for (auto &blob : blobs) {
			convexHulls.push_back(blob.contour);


		}

		 drawContours(imgConvexHulls, convexHulls, -1, SCALAR_WHITE, -1);

		// imshow("imgConvexHulls", imgConvexHulls);

		imgFrame2Copy = imgFrame2.clone();         

		for (auto &blob : blobs) {     

			int tlx = blob.boundingRect.tl().x - 25;
			int tly = blob.boundingRect.tl().y - 25;
			int brx = blob.boundingRect.br().x + 25;
			int bry = blob.boundingRect.br().y + 25;

			if (tlx <= 0) {
				tlx = 0;
			}


			if (tly <= 0) {
				tly = 0;
			}

			if (brx >= imgFrame2.cols) {
				brx = imgFrame2.cols;
			}

			if (bry >= imgFrame2.rows) {
				bry = imgFrame2.rows;
			}




			Rect pointCrop(Point(tlx,tly), Point(brx,bry));
			Mat crop = imgFrame3Copy(pointCrop);
			 //rectangle(imgFrame3Copy, pointCrop, SCALAR_RED, 2);
			 rectangle(imgFrame2Copy, blob.boundingRect, SCALAR_RED, 2);             
			 circle(imgFrame2Copy, blob.centerPosition, 3, SCALAR_GREEN, -1); 
			 ROI.push_back(crop);
		}

		 imshow("imgFrame2Copy", imgFrame2Copy);
		 imshow("imgFrame3Copy", imgFrame3Copy);
	

		imgFrame1 = imgFrame2.clone();           

		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {    
			capVideo.read(imgFrame2);                          
		}
		else {                                              
			 cout << "end of video\n";                     
			break;                                             
		}

		chCheckForEscKey =  waitKey(1);      

		HOG_SVM HOG_SVMHeader;
		if (ROI.size() != 0) {
			for (int i = 0; i < ROI.size(); i++) {

				if (ROI[i].cols < 64 || ROI[i].rows < 64) {
					continue;
				}
				HOG_SVMHeader.test_trained_detector(svmPath,ROI[i]);



			}
		}

		ROI.clear();
	}

	if (chCheckForEscKey != 27) {               
		 waitKey(0);                         
	}
	
	
}

void BGSubtraction::BackgroundSubtraction_old()
{
	Mat bg_im, bg_im_gray, image_gray, diff_im, bw, first_frame;
	double th = 50;
	char fileName[100] = "D:/Project_Oriental/Video Original/oriental/FE11.avi";
	VideoCapture stream1(fileName);
	stream1.read(bw);
	int i = 1;
	while (1)
	{
		Mat image;
		if (!(stream1.read(image))) {
			break;
		}

		stream1 >> image;
		Mat o_image = image.clone();


		if (i == 1)
		{
			first_frame = image;
			bg_im = first_frame;
			cvtColor(bg_im, bg_im_gray, COLOR_BGR2GRAY);
			GaussianBlur(bg_im_gray, bg_im_gray, Size(3, 3), 0, 0, BORDER_DEFAULT);
			namedWindow("BG", 1);
			imshow("BG", bg_im_gray);
		}
		cvtColor(image, image_gray, COLOR_BGR2GRAY);
		GaussianBlur(image_gray, image_gray, Size(3, 3), 0, 0, BORDER_DEFAULT);
		absdiff(bg_im_gray, image_gray, diff_im);
		threshold(diff_im, bw, th, 255, THRESH_BINARY);
		cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
		cv::dilate(bw, bw, structuringElement7x7);
		cv::dilate(bw, bw, structuringElement7x7);
		//cv::erode(bw, bw, structuringElement5x5);
		Mat SE(5, 5, CV_8U, Scalar(1));
		Mat cleaned_im;
		morphologyEx(bw, cleaned_im, MORPH_OPEN, SE);
		Mat diff_new, bw_new;
		cvtColor(diff_im, diff_new, COLOR_GRAY2BGR);
		cvtColor(bw, bw_new, COLOR_GRAY2BGR);
		HOG_SVM HOG_SVMHeader;

		std::vector<std::vector<Point>> contours;
		findContours(bw,
			contours,
			RETR_EXTERNAL,
			CHAIN_APPROX_NONE);

		vector<double> areas(contours.size());
		char buff[50];
		for (int i = 0; i < contours.size(); i++)
		{

			areas[i] = contourArea(contours[i]);

			printf("[%d] area=%.2f\n", i, areas[i]);

			Rect bb = boundingRect(contours[i]);
			int sx = bb.tl().x;
			int sy = bb.tl().y;
			int ex = bb.br().x - 1;
			int ey = bb.br().y - 1;
			int cx = (sx + ex) / 2;
			int cy = (sy + ey) / 2;

			if (areas[i] > 1500)
			{
				int tlx = bb.tl().x - 50;
				int tly = bb.tl().y - 50;
				int brx = bb.br().x + 50;
				int bry = bb.br().y + 50;

				if (tlx <= 0) {
					tlx = 0;
				}


				if (tly <= 0) {
					tly = 0;
				}

				if (brx >= image.cols) {
					brx = image.cols;
				}

				if (bry >= image.rows) {
					bry = image.rows;
				}

				Rect pointCrop(Point(tlx, tly), Point(brx, bry));
				Mat crop = o_image(pointCrop);

				//HOG_SVMHeader.test_trained_detector("D:/Senior_Project/head/my_detector4.yml", crop);
				rectangle(image, bb, Scalar(255, 0, 0), 2);
				drawMarker(image, Point(cx, cy), Scalar(255, 0, 0));
				//sprintf_s(buff, "Pedestrian");
				//putText(image, buff, Point(cx, cy + 20), 1, 1, Scalar(0, 0, 255), 1);

			}

		}
		imshow("Diff", diff_new);
		imshow("BW", bw_new);
		imshow("Image", image);
		i++;
		if (waitKey(20) >= 0)
			break;
	}
	destroyAllWindows();
}

Mat BGSubtraction::templeteMatching(vector<Mat> obj , Mat templ)
{
	Mat img; Mat result;
	String image_window = "Source Image";
	String result_window = "Result window";
	int match_method = CV_TM_SQDIFF_NORMED;
	namedWindow(image_window, CV_WINDOW_AUTOSIZE);
	namedWindow(result_window, CV_WINDOW_AUTOSIZE);

	/// Load image and template
	for (int i = 0; i < obj.size(); i++) {
		img = obj[i];

		/// Source image to display
		Mat img_display;
		img.copyTo(img_display);

		/// Create the result matrix
		int result_cols = img.cols - templ.cols + 1;
		int result_rows = img.rows - templ.rows + 1;

		result.create(result_rows, result_cols, CV_32FC1);

		/// Do the Matching and Normalize
		matchTemplate(img, templ, result, match_method);
		normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

		/// Localizing the best match with minMaxLoc
		double minVal; double maxVal; Point minLoc; Point maxLoc;
		Point matchLoc;

		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

		/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
		if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
		{
			matchLoc = minLoc;
		}
		else
		{
			matchLoc = maxLoc;
		}

		/// Show me what you got
		rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
		rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

		imshow(image_window, img_display);
		imshow(result_window, result);

		waitKey(0);

	}
	return Mat();
}

