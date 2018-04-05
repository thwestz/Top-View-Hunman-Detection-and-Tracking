#include "Tracking.h"
#include "samples_utility.cpp"
#include "Math.h"
#include "trackStructure.h"
MultiTracker Tracking::tracking_API(Mat frame, vector<Rect2d> ROIs, MultiTracker currentTrackers,vector<trackStructure> trackManage,int currentFrame) {
	vector<int> newTrackList;
	vector<Ptr<Tracker> > algorithms;
	MultiTracker temptrackers;
	//temptrackers.clear();
	temptrackers = currentTrackers;
	vector<Rect2d> objects;
	Mat originFrame = frame.clone();
	Mat visualStep = frame.clone();
	Point centerPosition;
	Mat path;
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
				if (((overlap.area() * 100.00) / totalSize.area() > 5.00)) {
					newTrackList.push_back(i);

					break;
				}

			}
		}
	}


	for (int z = 1; z < ROIs.size(); z++) {
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
		Point center_of_rect = (currentTrackers.getObjects()[i].br() + currentTrackers.getObjects()[i].tl())*0.5;
		trackStructure newTrack;
		newTrack.setTrackID(i+1);
		newTrack.setROI(currentTrackers.getObjects()[i]);
		newTrack.setStatus(1);
		//int s = newTrack.getStatus();
		//printf_s("%d", s);
		newTrack.setFirstFrame(currentFrame);
		newTrack.addCenterPoint(center_of_rect);
		trackManage.push_back(newTrack);
		printf_s("%d", newTrack.getTrackID());
		rectangle(frame, currentTrackers.getObjects()[i], Scalar(255, 0, 0), 2, 1);
		//printf_s("%d ",structure.getTracking_id());
		putText(frame, "id:" + to_string(newTrack.getTrackID()), currentTrackers.getObjects()[i].tl(), 1, 2, Scalar(255, 0, 255), 2);
		//circle(path, center_of_rect, 3, Scalar(0, 0, 255));
	
		//circle(frame, temptrackers.getObjects[i], 3, Scalar(0, 255, 0), -1);
	}

	//resize(frame, frame, Size(frame.cols / 3, frame.rows / 3));
	imshow("tracker", frame);
//	imshow("path", path);
	//waitKey(0);
	return currentTrackers;

}

