#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/objdetect.hpp"

#include <iostream>
#include <time.h>
#include "HOG_SVM.h"

using namespace cv;
using namespace cv::ml;
using namespace std;

Mat get_hogdescriptor_visu(const Mat& color_origImg, vector<float>& descriptorValues, const Size & size)
{
	const int DIMX = size.width;
	const int DIMY = size.height;
	float zoomFac = 3;
	Mat visu;
	resize(color_origImg, visu, Size((int)(color_origImg.cols*zoomFac), (int)(color_origImg.rows*zoomFac)));
	imshow("ASD", color_origImg);
	waitKey(0);
	int cellSize = 8;
	int gradientBinSize = 9;
	float radRangeForOneBin = (float)(CV_PI / (float)gradientBinSize); // dividing 180 into 9 bins, how large (in rad) is one bin?

																	   // prepare data structure: 9 orientation / gradient strenghts for each cell
	int cells_in_x_dir = DIMX / cellSize;
	int cells_in_y_dir = DIMY / cellSize;
	float*** gradientStrengths = new float**[cells_in_y_dir];
	int** cellUpdateCounter = new int*[cells_in_y_dir];
	for (int y = 0; y<cells_in_y_dir; y++)
	{
		gradientStrengths[y] = new float*[cells_in_x_dir];
		cellUpdateCounter[y] = new int[cells_in_x_dir];
		for (int x = 0; x<cells_in_x_dir; x++)
		{
			gradientStrengths[y][x] = new float[gradientBinSize];
			cellUpdateCounter[y][x] = 0;

			for (int bin = 0; bin<gradientBinSize; bin++)
				gradientStrengths[y][x][bin] = 0.0;
		}
	}

	// nr of blocks = nr of cells - 1
	// since there is a new block on each cell (overlapping blocks!) but the last one
	int blocks_in_x_dir = cells_in_x_dir - 1;
	int blocks_in_y_dir = cells_in_y_dir - 1;

	// compute gradient strengths per cell
	int descriptorDataIdx = 0;
	int cellx = 0;
	int celly = 0;

	for (int blockx = 0; blockx<blocks_in_x_dir; blockx++)
	{
		for (int blocky = 0; blocky<blocks_in_y_dir; blocky++)
		{
			// 4 cells per block ...
			for (int cellNr = 0; cellNr<4; cellNr++)
			{
				// compute corresponding cell nr
				cellx = blockx;
				celly = blocky;
				if (cellNr == 1) celly++;
				if (cellNr == 2) cellx++;
				if (cellNr == 3)
				{
					cellx++;
					celly++;
				}

				for (int bin = 0; bin<gradientBinSize; bin++)
				{
					float gradientStrength = descriptorValues[descriptorDataIdx];
					descriptorDataIdx++;

					gradientStrengths[celly][cellx][bin] += gradientStrength;

				} // for (all bins)


				  // note: overlapping blocks lead to multiple updates of this sum!
				  // we therefore keep track how often a cell was updated,
				  // to compute average gradient strengths
				cellUpdateCounter[celly][cellx]++;

			} // for (all cells)


		} // for (all block x pos)
	} // for (all block y pos)


	  // compute average gradient strengths
	for (celly = 0; celly<cells_in_y_dir; celly++)
	{
		for (cellx = 0; cellx<cells_in_x_dir; cellx++)
		{

			float NrUpdatesForThisCell = (float)cellUpdateCounter[celly][cellx];

			// compute average gradient strenghts for each gradient bin direction
			for (int bin = 0; bin<gradientBinSize; bin++)
			{
				gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
			}
		}
	}

	// draw cells
	for (celly = 0; celly<cells_in_y_dir; celly++)
	{
		for (cellx = 0; cellx<cells_in_x_dir; cellx++)
		{
			int drawX = cellx * cellSize;
			int drawY = celly * cellSize;

			int mx = drawX + cellSize / 2;
			int my = drawY + cellSize / 2;

			rectangle(visu, Point((int)(drawX*zoomFac), (int)(drawY*zoomFac)), Point((int)((drawX + cellSize)*zoomFac), (int)((drawY + cellSize)*zoomFac)), Scalar(100, 100, 100), 1);

			// draw in each cell all 9 gradient strengths
			for (int bin = 0; bin<gradientBinSize; bin++)
			{
				float currentGradStrength = gradientStrengths[celly][cellx][bin];

				// no line to draw?
				if (currentGradStrength == 0)
					continue;

				float currRad = bin * radRangeForOneBin + radRangeForOneBin / 2;

				float dirVecX = cos(currRad);
				float dirVecY = sin(currRad);
				float maxVecLen = (float)(cellSize / 2.f);
				float scale = 2.5; // just a visualization scale, to see the lines better

								   // compute line coordinates
				float x1 = mx - dirVecX * currentGradStrength * maxVecLen * scale;
				float y1 = my - dirVecY * currentGradStrength * maxVecLen * scale;
				float x2 = mx + dirVecX * currentGradStrength * maxVecLen * scale;
				float y2 = my + dirVecY * currentGradStrength * maxVecLen * scale;

				// draw gradient visualization
				line(visu, Point((int)(x1*zoomFac), (int)(y1*zoomFac)), Point((int)(x2*zoomFac), (int)(y2*zoomFac)), Scalar(0, 255, 0), 1);

			} // for (all bins)

		} // for (cellx)
	} // for (celly)


	  // don't forget to free memory allocated by helper data structures!
	for (int y = 0; y<cells_in_y_dir; y++)
	{
		for (int x = 0; x<cells_in_x_dir; x++)
		{
			delete[] gradientStrengths[y][x];
		}
		delete[] gradientStrengths[y];
		delete[] cellUpdateCounter[y];
	}
	delete[] gradientStrengths;
	delete[] cellUpdateCounter;

	imshow("HOG", visu);
	waitKey(0);
	return visu;

} // get_hogdescriptor_visu

void HOG_SVM::get_svm_detector(const Ptr< SVM >& svm, vector< float > & hog_detector)
{
	// get the support vectors
	Mat sv = svm->getSupportVectors();
	const int sv_total = sv.rows;
	// get the decision function
	Mat alpha, svidx;
	double rho = svm->getDecisionFunction(0, alpha, svidx);

	CV_Assert(alpha.total() == 1 && svidx.total() == 1 && sv_total == 1);
	CV_Assert((alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
		(alpha.type() == CV_32F && alpha.at<float>(0) == 1.f));
	CV_Assert(sv.type() == CV_32F);
	hog_detector.clear();

	hog_detector.resize(sv.cols + 1);
	memcpy(&hog_detector[0], sv.ptr(), sv.cols * sizeof(hog_detector[0]));
	hog_detector[sv.cols] = (float)-rho;
}

void HOG_SVM::convert_to_ml(const vector< Mat > & train_samples, Mat& trainData)
{
	//--Convert data
	const int rows = (int)train_samples.size();
	const int cols = (int)std::max(train_samples[0].cols, train_samples[0].rows);
	Mat tmp(1, cols, CV_32FC1); //< used for transposition if needed
	trainData = Mat(rows, cols, CV_32FC1);

	for (size_t i = 0; i < train_samples.size(); ++i)
	{
		CV_Assert(train_samples[i].cols == 1 || train_samples[i].rows == 1);

		if (train_samples[i].cols == 1)
		{
			transpose(train_samples[i], tmp);
			tmp.copyTo(trainData.row((int)i));
		}
		else if (train_samples[i].rows == 1)
		{
			train_samples[i].copyTo(trainData.row((int)i));
		}
	}
}

void HOG_SVM::load_images(const String & dirname, vector< Mat > & img_lst, bool showImages = false)
{
	vector< String > files;
	glob(dirname, files);

	for (size_t i = 0; i < files.size(); ++i)
	{
		Mat img = imread(files[i]); // load the image
		if (img.empty())            // invalid image, skip it.
		{
			cout << files[i] << " is invalid!" << endl;
			continue;
		}

		if (showImages)
		{
			imshow("image", img);
			waitKey(1);
		}
		img_lst.push_back(img);
	}
}

void HOG_SVM::sample_neg(const vector< Mat > & full_neg_lst, vector< Mat > & neg_lst, const Size & size)
{
	Rect box;
	box.width = size.width;
	box.height = size.height;
	RNG rng;
	rng(getTickCount());

	const int size_x = box.width;
	const int size_y = box.height;
	Mat nonfaceimage;
	srand((unsigned int)time(NULL));
	Size win_size(36, 36);
	int b = 0;
	for (size_t i = 0; i < full_neg_lst.size(); i++)
	{
		nonfaceimage = full_neg_lst[i].clone();
		for (int j = 0; j < 9; j++)
		{
			int rand_x = rng.uniform(0, nonfaceimage.cols - win_size.width - 1);
			int rand_y = rng.uniform(0, nonfaceimage.rows - win_size.height - 1);
			Mat NegImage = Mat(nonfaceimage, Rect(rand_x, rand_y, win_size.width, win_size.height));
			neg_lst.push_back(NegImage.clone());
			imwrite("C:/Users/Win10/Project/train/neg/image_n_" + to_string(b) + ".jpg", NegImage);
			b++;
		}
	}
}

void HOG_SVM::computeHOGs(const Size wsize, const vector< Mat > & img_lst, vector< Mat > & gradient_lst)
{
	//HOGDescriptor hog;
	//hog.winSize = wsize;

	HOGDescriptor hog(Size(36, 36), Size(8, 8), Size(4, 4), Size(4, 4), 9, 1, -1, HOGDescriptor::L2Hys, 0.2, false, HOGDescriptor::DEFAULT_NLEVELS, false);

	Rect r = Rect(0, 0, wsize.width, wsize.height);
	r.x += (img_lst[0].cols - r.width) / 2;
	r.y += (img_lst[0].rows - r.height) / 2;

	Mat gray;
	vector< float > descriptors;

	for (size_t i = 0; i < img_lst.size(); i++)
	{
		cvtColor(img_lst[i](r), gray, COLOR_BGR2GRAY);
		hog.compute(gray, descriptors);/*, Size(8, 8), Size(0, 0));*/
									   //get_hogdescriptor_visu(gray, descriptors, Size(64, 64));
		gradient_lst.push_back(Mat(descriptors).clone());
	}
}

vector<Rect> HOG_SVM::test_trained_detector(String obj_det_filename, Mat ROI, bool doHardNegative, int & counter)
{
	HOGDescriptor hog;
	vector<Rect> predicted;

	hog.load(obj_det_filename);

	int delay = 0;
	int k = 0;

	Mat img, img_o;

	img = ROI;
	if (img.empty())
	{
		return predicted;
	}
	vector< Rect > detections;
	vector< double > foundWeights;

	vector<Rect> found, found_filtered;
	Mat cleanImg = img.clone();
	double t = (double)getTickCount();

	///Oriental Video
	//hog.detectMultiScale(img, detections, foundWeights, 0, Size(4, 4), Size(8, 8), 1.01);
	///Floor 18
	hog.detectMultiScale(img, detections, foundWeights, 0, Size(0,0), Size(8, 8), 1.04);
		for (size_t j = 0; j < detections.size(); j++)
		{
			if (foundWeights[j] < 1.5) {
				continue;
			}
			Rect point(detections[j].tl(), detections[j].br());
			Scalar color = Scalar(0, foundWeights[j] * foundWeights[j] * 200, 0);
			predicted.push_back(point);
			//rectangle(img, detections[j], color, img.cols / 400 + 1);
			/*if (doHardNegative) {
				Mat crop = cleanImg(point);
				resize(crop, crop, Size(36, 36));
				imwrite("D:/hard_negative/image__" + to_string(counter) + ".jpg", crop);
				counter++;
			}*/
		}

		//imshow(obj_det_filename, img);
		return predicted;
}

int HOG_SVM::train_detector(String pos_dir, String neg_dir, String test_dir, String obj_det_filename, String videofilename)
{
	int detector_width = 36;
	int detector_height = 36;
	bool train_twice = false;
	bool visualization = false;

	vector< Mat > pos_lst, full_neg_lst, neg_lst, gradient_lst;
	vector< int > labels;


	load_images(pos_dir, pos_lst, visualization);
	if (pos_lst.size() > 0)
	{
	}
	else
	{
		printf_s("%s", "no image in \n");
		return 1;
	}

	Size pos_image_size = pos_lst[0].size();

	if (detector_width && detector_height)
	{
		pos_image_size = Size(detector_width, detector_height);
	}
	else
	{
		for (size_t i = 0; i < pos_lst.size(); ++i)
		{
			if (pos_lst[i].size() != pos_image_size)
			{
				printf_s("%s", "All positive images should be same size! \n");
				exit(1);
			}
		}

		pos_image_size = pos_image_size / 8 * 8;
	}

	labels.assign(pos_lst.size(), +1);
	const unsigned int old = (unsigned int)labels.size();

	printf_s("%s", "Negative images are being loaded... \n");
	load_images(neg_dir, full_neg_lst, false);
	sample_neg(full_neg_lst, neg_lst, pos_image_size);
	printf_s("%s $s", "...[done]", "\n");

	labels.insert(labels.end(), neg_lst.size(), -1);
	CV_Assert(old < labels.size());

	printf_s("%s $s","Histogram of Gradients are being calculated for positive images...", "\n");
	computeHOGs(pos_image_size, pos_lst, gradient_lst);
	printf_s("%s $s", "...[done]", "\n");

	printf_s("%s $s", "Histogram of Gradients are being calculated for negative images...", "\n");
	computeHOGs(pos_image_size, neg_lst, gradient_lst);
	printf_s("%s $s", "...[done]", "\n");

	Mat train_data;
	convert_to_ml(gradient_lst, train_data);

	printf_s("%s $s", "Training SVM...", "\n");
	Ptr< SVM > svm = SVM::create();
	/* Default values to train SVM */
	svm->setCoef0(0.0);
	svm->setDegree(3);
	svm->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 1e-3));
	svm->setGamma(0);
	svm->setKernel(SVM::LINEAR);
	svm->setNu(0.5);
	svm->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
	svm->setC(0.01); // From paper, soft classifier
	svm->setType(SVM::EPS_SVR); // C_SVC; // EPSILON_SVR; // may be also NU_SVR; // do regression task
	svm->train(train_data, ROW_SAMPLE, Mat(labels));
	printf_s("%s $s", "...[done]", "\n");

	if (train_twice)
	{
		printf_s("%s $s", "Testing trained detector on negative images. This may take a few minutes...", "\n");
		//HOGDescriptor my_hog;
		//my_hog.winSize = pos_image_size;
		HOGDescriptor my_hog(Size(36, 36), Size(8, 8), Size(4, 4), Size(4, 4), 9, 1, -1, HOGDescriptor::L2Hys, 0.2, false, HOGDescriptor::DEFAULT_NLEVELS, false);


		// Set the trained svm to my_hog
		vector< float > hog_detector;
		get_svm_detector(svm, hog_detector);
		my_hog.setSVMDetector(hog_detector);

		vector< Rect > detections;
		vector< double > foundWeights;

		for (size_t i = 0; i < full_neg_lst.size(); i++)
		{
			my_hog.detectMultiScale(full_neg_lst[i], detections, foundWeights);
			for (size_t j = 0; j < detections.size(); j++)
			{
				Mat detection = full_neg_lst[i](detections[j]).clone();
				resize(detection, detection, pos_image_size);
				neg_lst.push_back(detection);
			}
			if (visualization)
			{
				for (size_t j = 0; j < detections.size(); j++)
				{
					rectangle(full_neg_lst[i], detections[j], Scalar(0, 255, 0), 2);
				}
				imshow("testing trained detector on negative images", full_neg_lst[i]);
				waitKey(5);
			}
		}
		printf_s("%s $s", "...[done]", "\n");

		labels.clear();
		labels.assign(pos_lst.size(), +1);
		labels.insert(labels.end(), neg_lst.size(), -1);

		gradient_lst.clear();
		printf_s("%s $s", "Histogram of Gradients are being calculated for positive images...", "\n");
		computeHOGs(pos_image_size, pos_lst, gradient_lst);
		printf_s("%s $s", "...[done]", "\n");

		printf_s("%s $s", "Histogram of Gradients are being calculated for negative images...", "\n");
		computeHOGs(pos_image_size, neg_lst, gradient_lst);
		printf_s("%s $s", "...[done]", "\n");

		printf_s("%s $s", "Training SVM again...", "\n");
		convert_to_ml(gradient_lst, train_data);
		svm->train(train_data, ROW_SAMPLE, Mat(labels));
		printf_s("%s $s", "...[done]", "\n");
	}

	vector< float > hog_detector;
	get_svm_detector(svm, hog_detector);
	HOGDescriptor hog(Size(36, 36), Size(8, 8), Size(4, 4), Size(4, 4), 9, 1, -1, HOGDescriptor::L2Hys, 0.2, false, HOGDescriptor::DEFAULT_NLEVELS, false);
	hog.setSVMDetector(hog_detector);
	hog.save(obj_det_filename);

	return 0;
}
