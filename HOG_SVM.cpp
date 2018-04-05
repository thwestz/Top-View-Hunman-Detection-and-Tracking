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


	//hog.detectMultiScale(img, found, foundWeights, 0, Size(4,4), Size(0, 0),1.25);
	hog.detectMultiScale(img, detections, foundWeights, 0, Size(4, 4), Size(8, 8), 1.02);
	for (size_t j = 0; j < detections.size(); j++)
	{
		//printf_s("%f %s", foundWeights[j],"\n");
		if (foundWeights[j] < 1.0) {
			continue;
		}
		Rect point(detections[j].tl(), detections[j].br());
		Scalar color = Scalar(0, foundWeights[j] * foundWeights[j] * 200, 0);
		predicted.push_back(point);
		rectangle(img, detections[j], color, img.cols / 400 + 1);
		if (doHardNegative) {
		Mat crop = cleanImg(point);
		resize(crop, crop, Size(36, 36));
		imwrite("D:/hard_negative/image__"	+ to_string(counter) + ".jpg", crop);
		counter++;
		}
		//imshow("", img);
		//waitKey(0);
	}


	/*if (found.size() == 0) {
		return predicted;
	}else {


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
			/*if (doHardNegative) {
				Mat crop = cleanImg(point);
				resize(crop, crop, Size(64, 64));
				imwrite("D:/hard_negative/image_2_"

					+ to_string(counter) + ".jpg", crop);
				counter++;
			}*/
			//predicted.push_back(point);
		
	
	return predicted;
}

int HOG_SVM::TrainHOG_SVM(String pos_dir,String neg_dir , String test_dir ,String obj_det_filename)
{
	String videofilename = "";
	int detector_width = 36;
	int detector_height = 36;
	bool test_detector = false;
	bool train_twice = false;
	bool visualization = false;
	bool flip_samples = true;



	vector< Mat > pos_lst, full_neg_lst, neg_lst, gradient_lst;
	vector< int > labels;

	clog << "Positive images are being loaded...";
	load_images(pos_dir, pos_lst, visualization);
	if (pos_lst.size() > 0)
	{
		clog << "...[done]" << endl;
	}
	else
	{
		clog << "no image in " << pos_dir << endl;
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
				cout << "All positive images should be same size!" << endl;
				exit(1);
			}
		}
		pos_image_size = pos_image_size / 8 * 8;
	}

	clog << "Negative images are being loaded...";
	load_images(neg_dir, full_neg_lst, false);
	sample_neg(full_neg_lst, neg_lst, pos_image_size);
	clog << "...[done]" << endl;

	clog << "Histogram of Gradients are being calculated for positive images...";
	computeHOGs(pos_image_size, pos_lst, gradient_lst, flip_samples);
	size_t positive_count = gradient_lst.size();
	labels.assign(positive_count, +1);
	clog << "...[done] ( positive count : " << positive_count << " )" << endl;

	clog << "Histogram of Gradients are being calculated for negative images...";
	computeHOGs(pos_image_size, neg_lst, gradient_lst, flip_samples);
	size_t negative_count = gradient_lst.size() - positive_count;
	labels.insert(labels.end(), negative_count, -1);
	CV_Assert(positive_count < labels.size());
	clog << "...[done] ( negative count : " << negative_count << " )" << endl;

	Mat train_data;
	convert_to_ml(gradient_lst, train_data);

	clog << "Training SVM...";
	Ptr< SVM > svm = SVM::create();

	/* Default values to train SVM */
	svm->setCoef0(0.0);
	svm->setDegree(3);
	svm->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 1e-6));
	svm->setGamma(0.000000001);
	svm->setKernel(SVM::LINEAR);
	svm->setNu(0.5);
	svm->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
	svm->setC(0.01); // From paper, soft classifier
	svm->setType(SVM::NU_SVC); // C_SVC; // EPSILON_SVR; // may be also NU_SVR; // do regression task
	Ptr<TrainData> td = TrainData::create(train_data, ROW_SAMPLE, labels);
	svm->trainAuto(td);
	clog << "...[done]" << endl;

	if (train_twice)
	{
		clog << "Testing trained detector on negative images. This may take a few minutes...";
		HOGDescriptor my_hog(Size(36,36), Size(8, 8), Size(8, 8), Size(4, 4), 9, 1, -1, HOGDescriptor::L2Hys, 0.2, false, HOGDescriptor::DEFAULT_NLEVELS, false);

		my_hog.setSVMDetector(get_svm_detector(svm));

		vector< Rect > detections;
		vector< double > foundWeights;

		for (size_t i = 0; i < full_neg_lst.size(); i++)
		{
			if (full_neg_lst[i].cols >= pos_image_size.width && full_neg_lst[i].rows >= pos_image_size.height)
				my_hog.detectMultiScale(full_neg_lst[i], detections, foundWeights);
			else
				detections.clear();

			for (size_t j = 0; j < detections.size(); j++)
			{
				Mat detection = full_neg_lst[i](detections[j]).clone();
				resize(detection, detection, pos_image_size, 0, 0, INTER_LINEAR);
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
		clog << "...[done]" << endl;

		gradient_lst.clear();
		clog << "Histogram of Gradients are being calculated for positive images...";
		computeHOGs(pos_image_size, pos_lst, gradient_lst, flip_samples);
		positive_count = gradient_lst.size();
		clog << "...[done] ( positive count : " << positive_count << " )" << endl;

		clog << "Histogram of Gradients are being calculated for negative images...";
		computeHOGs(pos_image_size, neg_lst, gradient_lst, flip_samples);
		negative_count = gradient_lst.size() - positive_count;
		clog << "...[done] ( negative count : " << negative_count << " )" << endl;

		labels.clear();
		labels.assign(positive_count, +1);
		labels.insert(labels.end(), negative_count, -1);

		clog << "Training SVM again...";
		convert_to_ml(gradient_lst, train_data);
		svm->train(train_data, ROW_SAMPLE, labels);
		clog << "...[done]" << endl;
	}

	HOGDescriptor hog(Size(36, 36), Size(8, 8), Size(8, 8), Size(4, 4), 9, 1, -1, HOGDescriptor::L2Hys, 0.2, false, HOGDescriptor::DEFAULT_NLEVELS, false);

	hog.setSVMDetector(get_svm_detector(svm));
	hog.save(obj_det_filename);

	return 0;
}
