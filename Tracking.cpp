#include "Tracking.h"
#include "samples_utility.cpp"
#include "Math.h"
#include "trackStructure.h"


MultiTracker Tracking::tracking_API(Mat frame, vector<Rect2d> ROIs, MultiTracker currentTrackers,int currentFrame) {
	vector<int> newTrackList;
	vector<Ptr<Tracker> > algorithms;
	MultiTracker temptrackers;
	temptrackers = currentTrackers;
	vector<Rect2d> objects;

	if (currentTrackers.getObjects().size() == 0) {
		for (size_t i = 0; i < ROIs.size(); i++)
		{
			algorithms.push_back(createTrackerByName("MIL"));
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

	return currentTrackers;

}

vector<trackStructure> Tracking::initalID(Mat toTrackimg, MultiTracker currentTrackers, int currentFrame){
	vector<trackStructure> currentTrackStruture;
	for (int i = 0; i < currentTrackers.getObjects().size(); i++) {
		Point center_of_rect = (currentTrackers.getObjects()[i].br() + currentTrackers.getObjects()[i].tl())*0.5;
		trackStructure newTrack;
		newTrack.setTrackID(i + 1);
		newTrack.setROI(currentTrackers.getObjects()[i]);
		newTrack.setStatus(1);
		newTrack.setFirstFrame(currentFrame);
		newTrack.setCenterPoint(center_of_rect);
		currentTrackStruture.push_back(newTrack);

		rectangle(toTrackimg, currentTrackers.getObjects()[i], Scalar(255, 0, 0), 2, 1);
		putText(toTrackimg, "id:" + to_string(newTrack.getTrackID()), currentTrackers.getObjects()[i].tl(), 1, 2, Scalar(255, 0, 255), 2);

		imshow("T", toTrackimg);

	
	}

	return currentTrackStruture;

}