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
		currentTrackers.add(algorithms, frame, objects);
	}
	else if (ROIs.size() > currentTrackers.getObjects().size() || ROIs.size() < currentTrackers.getObjects().size()) {
		for (size_t i = 0; i < ROIs.size(); i++)
		{
			for (size_t j = 0; j < currentTrackers.getObjects().size(); j++)
			{
				Rect2d overlap = ROIs[i] & currentTrackers.getObjects()[j];
				Rect2d totalSize = ROIs[i] | currentTrackers.getObjects()[j];
				//printf_s("%s %f %s", "OVERLAP", overlap.area(), "\n");
				if ((overlap.area() * 100 ) / totalSize.area() > 20.00) {
					printf_s("%s %f %s", "OVERLAP", (overlap.area() * 100) / totalSize.area(), "\n");
					Ptr<Tracker>  newAlgor;
					newAlgor = createTrackerByName("MIL");
					//objects.push_back(ROIs[i]);
					currentTrackers.add(newAlgor, frame, ROIs[i]);
					break;
				}
			}
		}

	}



	//currentTrackers.add(algorithms, frame, objects);
	currentTrackers.update(frame);

	for (unsigned i = 0; i < currentTrackers.getObjects().size(); i++) {
		rectangle(frame, currentTrackers.getObjects()[i], Scalar(255, 0, 0), 2, 1);
		//circle(frame, temptrackers.getObjects[i], 3, Scalar(0, 255, 0), -1);
	}
	//resize(frame, frame, Size(frame.cols / 3, frame.rows / 3));
	imshow("tracker", frame);
	return currentTrackers;

}