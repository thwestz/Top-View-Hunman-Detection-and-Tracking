/*----------------------------------------------
* Usage:
* example_tracking_multitracker <video_name> [algorithm]
*
* example:
* example_tracking_multitracker Bolt/img/%04d.jpg
* example_tracking_multitracker faceocc2.webm KCF
*--------------------------------------------------*/

#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "samples_utility.cpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
	// show help
	if (argc<2) {
		cout <<
			" Usage: example_tracking_multitracker <video_name> [algorithm]\n"
			" examples:\n"
			" example_tracking_multitracker Bolt/img/%04d.jpg\n"
			" example_tracking_multitracker faceocc2.webm MEDIANFLOW\n"
			<< endl;
		return 0;
	}

	// set the default tracking algorithm
	std::string trackingAlg = "MIL";

	// set the tracking algorithm from parameter
	if (argc>2)
		trackingAlg = argv[2];

	// create the tracker
	MultiTracker trackers;

	// container of the tracked objects
	vector<Rect2d> objects;

	// set input video
	std::string video = argv[1];
	VideoCapture cap(video);

	Mat frame;
	cap.set(CAP_PROP_POS_FRAMES, 375);
	// get bounding box
	cap >> frame;
	vector<Rect> ROIs;
	selectROIs("tracker", frame, ROIs);

	//quit when the tracked object(s) is not provided
	if (ROIs.size()<1)
		return 0;

	// initialize the tracker
	std::vector<Ptr<Tracker> > algorithms;
	for (size_t i = 0; i < ROIs.size(); i++)
	{
		algorithms.push_back(createTrackerByName(trackingAlg));
		objects.push_back(ROIs[i]);
	}

	trackers.add(algorithms, frame, objects);

	// do the tracking
	printf("Start the tracking process, press ESC to quit.\n");
	for (;; ) {
		// get frame from the video
		cap >> frame;

		// stop the program if no more images
		if (frame.rows == 0 || frame.cols == 0)
			break;

		//update the tracking result
		trackers.update(frame);

		// draw the tracked object
		for (unsigned i = 0; i<trackers.getObjects().size(); i++)
			rectangle(frame, trackers.getObjects()[i], Scalar(255, 0, 0), 2, 1);

		// show image with the tracked object
		imshow("tracker", frame);

		//quit on ESC button
		if (waitKey(1) == 27)break;
	}

}