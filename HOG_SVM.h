#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/objdetect.hpp"

#include <iostream>
#include <time.h>

using namespace cv;
using namespace cv::ml;
using namespace std;

class HOG_SVM {
public:

	void get_svm_detector(const Ptr< SVM > & svm, vector< float > & hog_detector);
	void convert_to_ml(const std::vector< Mat > & train_samples, Mat& trainData);
	void load_images(const String & dirname, vector< Mat > & img_lst, bool showImages);
	void sample_neg(const vector< Mat > & full_neg_lst, vector< Mat > & neg_lst, const Size & size);
	void computeHOGs(const Size wsize, const vector< Mat > & img_lst, vector< Mat > & gradient_lst);
	vector<Rect> test_trained_detector(String obj_det_filename, Mat ROI, bool doHardNegative, int & counter);
	int train_detector(String pos_dir, String neg_dir, String test_dir, String obj_det_filename, String videofilename);
};
