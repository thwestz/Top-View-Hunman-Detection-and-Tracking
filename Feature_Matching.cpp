#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;


Mat Compute_Homography(vector<Point2f> srcPts, vector<Point2f> dstPts);

int main()
{
	//----------------------------------------------
	// Training step:
	//
	// Read the reference image
	// 
	Mat img_ref = imread("D:/Computer Vision/1043_Chaiwat_HW02/pic2-1.jpg", IMREAD_GRAYSCALE);
	// Specify the ROI of interest field
	Rect roi;
	roi.x = 250;
	roi.y = 180;
	roi.width = 80;
	roi.height = 25;

	// Detect the keypoints and extract descriptors of the reference image
	Ptr<SIFT> detector = SIFT::create();
	std::vector<KeyPoint> keypoints_ref, keypoints_scene;

	Mat descriptors_ref;

	detector->detectAndCompute(img_ref, Mat(), keypoints_ref, descriptors_ref);
	Mat img_ref_keypoints;
	drawKeypoints(img_ref, keypoints_ref, img_ref_keypoints);

	// Display the keypoint in the reference document image
	imshow("Reference Document Image", img_ref_keypoints);

	//----------------------------------------------
	// Testing phase

	// Read the test image
	Mat img_scene = imread("D:/Computer Vision/1043_Chaiwat_HW02/pic2-3.jpg", IMREAD_GRAYSCALE);

	// Detect the keypoints and extract descriptors of the test image
	Mat descriptors_scene;
	detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);

	// Display the keypoint in the test image
	Mat img_scene_keypoints;
	drawKeypoints(img_scene, keypoints_scene, img_scene_keypoints);
	imshow("Test Image", img_scene_keypoints);


	//  Feature matching between the reference document image and the test image
	BFMatcher matcher(NORM_L2);
	std::vector< DMatch > matches;
	matcher.match(descriptors_ref, descriptors_scene, matches);

	Mat img_matches;
	drawMatches(img_ref, keypoints_ref, img_scene, keypoints_scene, matches, img_matches);

	imshow("Putative matches", img_matches);
	waitKey(0);

	//===============================
	//-- RANSAC Homography estimation to localize the form area in the test image

	//------------------------------------------------------------------
	//// Start of RANSAC loop
	vector<Point2f> src_pts, dst_pts;
	vector<int> inlier_idx;

	// Variable declaration
	RNG rng; // variable for Random number generator
	int iter, n_req_iters, i, j, n_pts, n_model_points, inlier_cnt,
		best_inlier_cnt;
	vector<int> idx;
	vector<Point2f> sel_src_pts, sel_dst_pts, b_sel_src_pts, b_sel_dst_pts;
	vector<double> err, best_err;
	vector<uchar> inlier_mask, best_inlier_mask;
	vector<DMatch> inlier_matches;
	Mat best_Ha;

	rng(getTickCount()); // Seed the random number generator
	n_model_points = 4; // Number of minimum points
						// required to calculate the transformation
	n_pts = matches.size(); // Number of data points

	inlier_mask.resize(n_pts); // Mask for inlier: 0=outlier, 1=inlier
	best_inlier_mask.resize(n_pts); //
	err.resize(n_pts); // Error for each match pair
	best_err.resize(n_pts);
	best_Ha.create(2, 3, CV_64F);
	idx.resize(n_model_points);

	idx.resize(n_model_points); // Indexes of data points used
								// in each iteration
	best_inlier_cnt = -1;
	n_req_iters = 1000; // Initially set n_req_iters to 1000
	printf("[0] n_req_iters=%d\n", n_req_iters);

	src_pts.resize(n_pts);
	dst_pts.resize(n_pts);

	for (int i = 0; i < n_pts; i++) {
		src_pts[i] = Point2f(keypoints_ref[matches[i].queryIdx].pt.x, keypoints_ref[matches[i].queryIdx].pt.y);
		dst_pts[i] = Point2f(keypoints_scene[matches[i].trainIdx].pt.x, keypoints_scene[matches[i].trainIdx].pt.y);
	}

	//------------------------------------------------------------------
	//// Start of RANSAC loop
	for (iter = 0; iter < n_req_iters; iter++)
	{
		// Step 1: Randomly select 4 pairs of matchings
		for (i = 0; i < n_model_points; i = i + 1)
		{
			for (;;) {
				// Generate a random number between 0 to n_pts-1
				idx[i] = rng.uniform(0, n_pts - 1);
				// Check if the generated number is repeated to
				// the previous one, if yes – re-generate another
				for (j = 0; j < i; j++) {
					if (idx[i] == idx[j])
					{
						break;
					}
				}
				if (j == i) {
					break;
				}
			}
		}
		// 


		sel_src_pts.resize(n_model_points);
		sel_dst_pts.resize(n_model_points);
		
		// Step 2: Compute the homography transformation from the selected 4 pairs
		for (i = 0; i < n_model_points; i = i + 1) {
			sel_src_pts[i] = src_pts[idx[i]];
			sel_dst_pts[i] = dst_pts[idx[i]];
		}
		Mat Ha = Compute_Homography(sel_src_pts, sel_dst_pts);


		// Step 3: Compute Error of all matching
		// decide if each point is inlier or outlier

		double reProjTh = 5;

		inlier_cnt = 0;
		for (i = 0; i < n_pts; i = i + 1) {
			double x = src_pts[i].x;
			double y = src_pts[i].y;

			Mat p(3, 1, CV_64FC1);
			p.at<double>(0, 0) = x;
			p.at<double>(1, 0) = y;
			p.at<double>(2, 0) = 1;
			Mat pp = Ha * p;

			double xp = pp.at<double>(0, 0) / pp.at<double>(2, 0);
			double yp = pp.at<double>(1, 0) / pp.at<double>(2, 0);

			double a = xp - dst_pts[i].x;
			double b = yp - dst_pts[i].y;

			err[i] = a*a + b*b;
			if (err[i] < reProjTh*reProjTh) {
				inlier_mask[i] = 1;
				inlier_cnt++;
			}
			else
				inlier_mask[i] = 0;
		}
		// Step 4: Check if the current homography is better than the one that we kept
		// Adaptively re-compute the number of iterations required to do random sampling
		if (inlier_cnt > best_inlier_cnt) {
			best_inlier_cnt = inlier_cnt;
			Ha.copyTo(best_Ha);
			best_err.resize(n_pts);
			best_inlier_mask.resize(n_pts);
			for (i = 0; i < n_pts; i++) {
				best_err[i] = err[i];
				best_inlier_mask[i] = inlier_mask[i];
			}
			printf("[%d] best_inlier_cnt=%d\n", iter, best_inlier_cnt);
			// Adaptive re-compute the number of iterations
			// required to do random sampling
			double confidence_p, outlier_ratio_e, numer, denom;
			confidence_p = 0.90;
			outlier_ratio_e = (double)(n_pts - best_inlier_cnt) / n_pts;
			numer = log(1 - confidence_p + DBL_EPSILON);
			double tmp = pow((1.0 - outlier_ratio_e), n_model_points);
			denom = log(1 - tmp + DBL_EPSILON);
			n_req_iters = round(numer / denom);
			printf("[%d] n_req_iters=%d\n", iter, n_req_iters);
		}

	}
	printf("# iters actually run = %d\n", iter);
	printf("best inlier cnt = %d\n", best_inlier_cnt);
	//// End of RANSAC loop
	//------------------------------------------------------------------

	// Display the inlier matches detected by RANSAC
	// Also display the four corners of the form area in the test image
	vector<DMatch> matchesRANSAC;
	matchesRANSAC.resize(best_inlier_cnt);
	int qq = 0;
	for (int i = 0; i < n_pts; i++) {
		if (best_inlier_mask[i] == 1) {
			matchesRANSAC[qq++] = matches[i];
		}
	}

	Mat img_matchesRANSAC;
	drawMatches(img_ref, keypoints_ref, img_scene, keypoints_scene, matchesRANSAC, img_matchesRANSAC);

	imshow("RANSAC", img_matchesRANSAC);
	waitKey(0);
	//find corners
	vector<Point2f> ref_corners(4);
	ref_corners[0] = Point(0, 0);
	ref_corners[1] = Point(img_ref.cols, 0);
	ref_corners[2] = Point(img_ref.cols, img_ref.rows);
	ref_corners[3] = Point(0, img_ref.rows);


	vector<Point2f> scene_corners(4);
	for (int i = 0; i < scene_corners.size(); i++) {
		Mat p(3, 1, CV_64FC1);
		p.at<double>(0, 0) = ref_corners[i].x;
		p.at<double>(1, 0) = ref_corners[i].y;
		p.at<double>(2, 0) = 1;
		Mat pp = best_Ha * p;
		scene_corners[i].x = pp.at<double>(0, 0) / pp.at<double>(2, 0);
		scene_corners[i].y = pp.at<double>(1, 0) / pp.at<double>(2, 0);
	}
	Mat img_scene_corners;
	cvtColor(img_scene, img_scene_corners, COLOR_GRAY2BGR);
	line(img_scene_corners, scene_corners[0], scene_corners[1], Scalar(0, 255, 0), 4);
	line(img_scene_corners, scene_corners[1], scene_corners[2], Scalar(0, 255, 0), 4);
	line(img_scene_corners, scene_corners[2], scene_corners[3], Scalar(0, 255, 0), 4);
	line(img_scene_corners, scene_corners[3], scene_corners[0], Scalar(0, 255, 0), 4);

	Mat img_ref_roi = img_ref.clone();
	line(img_ref_roi, Point(roi.x, roi.y), Point(roi.x + roi.width, roi.y), Scalar(0, 255, 0), 2);
	line(img_ref_roi, Point(roi.x + roi.width, roi.y), Point(roi.x + roi.width, roi.y + roi.height), Scalar(0, 255, 0), 2);
	line(img_ref_roi, Point(roi.x + roi.width, roi.y + roi.height), Point(roi.x, roi.y + roi.height), Scalar(0, 255, 0), 2);
	line(img_ref_roi, Point(roi.x, roi.y + roi.height), Point(roi.x, roi.y), Scalar(0, 255, 0), 2);

	
	vector<Point2i> ref_roi_corners(4);
	ref_roi_corners[0] = Point(roi.x, roi.y);
	ref_roi_corners[1] = Point(roi.x + roi.width, roi.y);
	ref_roi_corners[2] = Point(roi.x + roi.width, roi.y + roi.height);
	ref_roi_corners[3] = Point(roi.x, roi.y + roi.height);

	Mat img_scene_roi_corners;
	cvtColor(img_scene, img_scene_roi_corners, COLOR_GRAY2BGR);

	vector<Point2i> scene_roi_corners(4);
	for (int i = 0; i < scene_corners.size(); i++) {
		Mat p(3, 1, CV_64FC1);
		p.at<double>(0, 0) = ref_roi_corners[i].x;
		p.at<double>(1, 0) = ref_roi_corners[i].y;
		p.at<double>(2, 0) = 1;
		Mat pp = best_Ha * p;
		scene_roi_corners[i].x = pp.at<double>(0, 0) / pp.at<double>(2, 0);
		scene_roi_corners[i].y = pp.at<double>(1, 0) / pp.at<double>(2, 0);
	}

	line(img_scene_roi_corners, scene_roi_corners[0], scene_roi_corners[1], Scalar(0, 255, 0), 2);
	line(img_scene_roi_corners, scene_roi_corners[1], scene_roi_corners[2], Scalar(0, 255, 0), 2);
	line(img_scene_roi_corners, scene_roi_corners[2], scene_roi_corners[3], Scalar(0, 255, 0), 2);
	line(img_scene_roi_corners, scene_roi_corners[3], scene_roi_corners[0], Scalar(0, 255, 0), 2);

	// Calculate the ROI of interest field by using the homography
	Mat field(roi.size(), CV_8UC1);
	for (int y = 0; y < field.rows; y++) {
		for (int x = 0; x < field.cols; x++) {
			Mat p(3, 1, CV_64FC1);
			p.at<double>(0, 0) = x + roi.x;
			p.at<double>(1, 0) = y + roi.y;
			p.at<double>(2, 0) = 1;
			Mat pp = best_Ha * p;
			int xp = pp.at<double>(0, 0) / pp.at<double>(2, 0);
			int yp = pp.at<double>(1, 0) / pp.at<double>(2, 0);
			double a = ceil(xp) - xp;
			double b = ceil(yp) - yp;
			int  i = xp;
			int  j = yp;
			int pixel = 0;
			pixel += a*b *img_scene.at<uchar>(j, i);
			pixel += (1 - a)*b *img_scene.at<uchar>(j, i + 1);
			pixel += a*(1 - b) *img_scene.at<uchar>(j + 1, i);
			pixel += (1 - a)*(1 - b) *img_scene.at<uchar>(j + 1, i + 1);
			field.at<uchar>(y, x) = saturate_cast<uchar>(pixel);

		}
	}
	imshow("Form Extraction : Test ROI", img_scene_roi_corners);
	imshow("ROI", field);
	waitKey(0);
	// Warp the image in the ROI of interest field into the rectified image

	// Display the warped ROI
	Mat img_DocROI;
	drawMatches(img_ref, keypoints_ref, img_scene_corners, keypoints_scene, matchesRANSAC, img_DocROI);
	imshow("Form Extraction: Document ROI", img_DocROI);
	waitKey(0);
	return 0;
}


Mat Compute_Homography(vector<Point2f> srcPts, vector<Point2f> dstPts)
{
	int n = 4;
	Mat A, B;
	A = Mat::zeros(2 * n, 9, CV_64FC1);
	B = Mat::zeros(2 * n, 1, CV_64FC1);
	for (int i = 0; i < 4; i++)
	{
		float x = srcPts[i].x;
		float y = srcPts[i].y;
		float xp = dstPts[i].x;
		float yp = dstPts[i].y;

		A.at<double>((2 * i) + 0, 0) = x;
		A.at<double>((2 * i) + 0, 1) = y;
		A.at<double>((2 * i) + 0, 2) = 1;
		A.at<double>((2 * i) + 0, 6) = -xp * x;
		A.at<double>((2 * i) + 0, 7) = -xp * y;
		A.at<double>((2 * i) + 0, 8) = -xp;
		A.at<double>((2 * i) + 1, 3) = x;
		A.at<double>((2 * i) + 1, 4) = y;
		A.at<double>((2 * i) + 1, 5) = 1;
		A.at<double>((2 * i) + 1, 6) = -yp*x;
		A.at<double>((2 * i) + 1, 7) = -yp*y;
		A.at<double>((2 * i) + 1, 8) = -yp;

		B.at<double>((2 * i) + 0, 0) = 0;
		B.at<double>((2 * i) + 1, 0) = 0;
	}

	Mat xvec;
	Mat AtA;
	mulTransposed(A, AtA, true);
	Mat H(3, 3, CV_64F);
	Mat eigenVa, eigenVec;
	eigen(AtA, eigenVa, eigenVec);

	int k = 0;
	for (int j = 0; j < H.rows; j++) {
		for (int i = 0; i < H.cols; i++) {
			H.at<double>(j, i) = eigenVec.at<double>(8, k);
			k++;
		}
	}

	return H;
}

