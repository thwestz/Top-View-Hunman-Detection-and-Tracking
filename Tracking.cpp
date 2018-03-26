#include "Tracking.h"
#include "samples_utility.cpp"
#include "Math.h"

MultiTracker Tracking::tracking_API(Mat frame, vector<Rect2d> ROIs, MultiTracker currentTrackers) {

	vector<Ptr<Tracker> > algorithms;
	MultiTracker temptrackers;
	//temptrackers.clear();
	temptrackers = currentTrackers;
	vector<Rect2d> objects;
	Point centerPosition;

	//first time to add object
	if (currentTrackers.getObjects().size() == 0) {
		for (size_t i = 0; i < ROIs.size(); i++)
		{
			algorithms.push_back(createTrackerByName("MIL"));
			//Rect2d resizeROI(Point(ROIs[i].tl().x + 30, ROIs[i].tl().y + 30),Point(ROIs[i].br().x + 30 , ROIs[i].br().y + 30));
			objects.push_back(ROIs[i]);
			continue;
		}
	}
	else {
		for (size_t i = 0; i < ROIs.size(); i++)
		{
			Rect2d overlap = ROIs[i] & currentTrackers.getObjects()[i];
			if (overlap.area() > 0) {
				printf_s("%f", overlap.area());
				/*algorithms.push_back(createTrackerByName("KCF"));
				objects.push_back(currentTrackers.getObjects()[i]);
				continue;*/
				break;
			}
		}
	}

	currentTrackers.add(algorithms, frame, objects);
	currentTrackers.update(frame);

	for (unsigned i = 0; i < currentTrackers.getObjects().size(); i++) {
		rectangle(frame, currentTrackers.getObjects()[i], Scalar(255, 0, 0), 2, 1);
		//circle(frame, temptrackers.getObjects[i], 3, Scalar(0, 255, 0), -1);
	}
	//resize(frame, frame, Size(frame.cols / 3, frame.rows / 3));
	imshow("tracker", frame);
	return currentTrackers;

}