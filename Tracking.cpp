#include "Tracking.h"
#include "samples_utility.cpp"
#include "Math.h"

MultiTracker Tracking::tracking_API(Mat frame, vector<Rect2d> ROIs, MultiTracker currentTrackers) {
	vector<int> newTrackList;
	vector<Ptr<Tracker> > algorithms;
	MultiTracker temptrackers;
	//temptrackers.clear();
	temptrackers = currentTrackers;
	vector<Rect2d> objects;
	Mat originFrame = frame.clone();
	Mat visualStep = frame.clone();
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
	else {
		for (int i = 0; i < ROIs.size(); i++) {

			for (int j = 0; j < currentTrackers.getObjects().size(); j++)
			{

				Rect2d overlap = ROIs[i] & currentTrackers.getObjects()[j];
				Rect2d totalSize = ROIs[i] | currentTrackers.getObjects()[j];
				if (((overlap.area() * 100.00) / totalSize.area() > 10.00)) {
					newTrackList.push_back(i);

					break;
				}

			}
		}
	}


	for (int z = 0; z < ROIs.size(); z++) {
		bool flag = false;
		for (int x = 0; x < newTrackList.size(); x++) {

			if (z == newTrackList[x]) {
				flag = true;
				break;
			}

		}
		if (flag == false) {
			Ptr<Tracker>  newAlgor;
			newAlgor = createTrackerByName("MIL");
			currentTrackers.add(newAlgor, frame, ROIs[z]);
		}
	}
	newTrackList.clear();


	currentTrackers.update(frame);

	for (unsigned i = 0; i < currentTrackers.getObjects().size(); i++) {
		rectangle(frame, currentTrackers.getObjects()[i], Scalar(255, 0, 0), 2, 1);
		//circle(frame, temptrackers.getObjects[i], 3, Scalar(0, 255, 0), -1);
	}
	//resize(frame, frame, Size(frame.cols / 3, frame.rows / 3));
	imshow("tracker", frame);
	//waitKey(0);
	return currentTrackers;

}

