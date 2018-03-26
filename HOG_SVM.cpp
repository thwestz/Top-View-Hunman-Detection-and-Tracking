#include "HOG_SVM.h"
vector< float > HOG_SVM::get_svm_detector(const Ptr< SVM >& svm)
{
	Mat sv = svm->getSupportVectors();
	const int sv_total = sv.rows;
	Mat alpha, svidx;
	double rho = svm->getDecisionFunction(0, alpha, svidx);

	CV_Assert(alpha.total() == 1 && svidx.total() == 1 && sv_total == 1);
	CV_Assert((alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
		(alpha.type() == CV_32F && alpha.at<float>(0) == 1.f));
	CV_Assert(sv.type() == CV_32F);

	vector< float > hog_detector(sv.cols + 1);
	memcpy(&hog_detector[0], sv.ptr(), sv.cols * sizeof(hog_detector[0]));
	hog_detector[sv.cols] = (float)-rho;
	return hog_detector;
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

	srand((unsigned int)time(NULL));

	for (size_t i = 0; i < full_neg_lst.size(); i++)
		neg_lst.push_back(full_neg_lst[i]);
}

void HOG_SVM::computeHOGs(const Size wsize, const vector< Mat > & img_lst, vector< Mat > & gradient_lst, bool use_flip)
{
	HOGDescriptor hog;

	hog.winSize = wsize;
	Mat gray;
	vector< float > descriptors;

	for (size_t i = 0; i < img_lst.size(); i++)
	{
		if (img_lst[i].cols >= wsize.width && img_lst[i].rows >= wsize.height)
		{
			Rect r = Rect((img_lst[i].cols - wsize.width) / 2,
				(img_lst[i].rows - wsize.height) / 2,
				wsize.width,
				wsize.height);
			cvtColor(img_lst[i](r), gray, COLOR_BGR2GRAY);
			hog.compute(gray, descriptors, Size(8, 8), Size(0, 0));
			gradient_lst.push_back(Mat(descriptors).clone());
			if (use_flip)
			{
				flip(gray, gray, 1);
				hog.compute(gray, descriptors, Size(8, 8), Size(0, 0));
				gradient_lst.push_back(Mat(descriptors).clone());
			}
		}
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

	hog.detectMultiScale(img, found, 0, Size(8, 8), Size(0, 0), 1.25);
	if (found.size() != 0) {


		for (size_t i = 0; i < found.size(); i++)
		{
			Rect r = found[i];

			size_t j;

			for (j = 0; j < found.size(); j++)
				if (j != i && (r & found[j]) == r)
					break;

			if (j == found.size())
				found_filtered.push_back(r);
		}
		for (size_t i = 0; i < found_filtered.size(); i++)
		{
			Rect r = found_filtered[i];

			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			Rect point(r.tl(), r.br());
			if (doHardNegative) {
				Mat crop = cleanImg(point);
				resize(crop, crop, Size(64, 64));
				imwrite("D:/hard_negative/image_2_"
					+ to_string(counter) + ".jpg", crop);
				counter++;
			}
			predicted.push_back(point);
		}
	}
	return predicted;
}
