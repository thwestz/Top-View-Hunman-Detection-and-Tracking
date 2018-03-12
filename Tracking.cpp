#include "Tracking.h"
#include "samples_utility.cpp"

vector<Rect2d> Tracking::tracking_API(Mat frame , vector<Rect> ROIs , String trackingAlg
, vector<Ptr<Tracker> > algorithms , vector<Rect2d> objects, MultiTracker trackers) {



		for (size_t i = 0; i < ROIs.size(); i++)
		{
			algorithms.push_back(createTrackerByName(trackingAlg));
			objects.push_back(ROIs[i]);
			trackers.add(algorithms[i], frame, objects[i]);
		}

		 trackers.update(frame);


		for (unsigned i = 0; i<trackers.getObjects().size(); i++)
			rectangle(frame, trackers.getObjects()[i], Scalar(255, 0, 0), 2, 1);

		resize(frame,frame,Size(frame.cols/2,frame.rows/2));
		imshow("tracker", frame);

	
	return trackers.getObjects();
}