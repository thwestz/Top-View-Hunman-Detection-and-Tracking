#include "Tracking.h"
MultiTracker Tracking::adaptMultiTracker(Mat frame, vector<Rect2d> ROIs, MultiTracker currentTrackers, int currentFrame) {
	vector<int> eraseTrackList;
	vector<Ptr<Tracker> > algorithms;
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
					eraseTrackList.push_back(i);

					break;
				}

			}
		}
	}

	for (int z = 1; z < ROIs.size(); z++) {
		bool flag = false;
		for (int x = 0; x < eraseTrackList.size(); x++) {

			if (z == eraseTrackList[x]) {
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
	eraseTrackList.clear();

	currentTrackers.update(frame);



	return currentTrackers;

}

vector<trackStructure> Tracking::initalID(MultiTracker currentTrackers, int currentFrame) {

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
	}


	return currentTrackStruture;

}

vector<reportTracking> Tracking::manageReport(vector<trackStructure> currentTrackStruture, vector<reportTracking> pathList)
{

	for (int k = 0; k < currentTrackStruture.size(); k++) {
		if (pathList.size() > 0) {
			for (int i = 0; i < pathList.size(); i++) {

				if (currentTrackStruture[k].getTrackID() == pathList[i].getID()) {
					pathList[i].addPath(currentTrackStruture[k].getCenterPoint());
					pathList[i].addPathRect(currentTrackStruture[k].getROI());
					break;
				}

				if (currentTrackStruture[k].getTrackID() != pathList[i].getID() &&
					currentTrackStruture[k].getTrackID() > pathList.size()) {
					reportTracking reporter;

					reporter.setID(currentTrackStruture[k].getTrackID());
					reporter.addPath(currentTrackStruture[k].getCenterPoint());
					reporter.addPathRect(currentTrackStruture[k].getROI());
					pathList.push_back(reporter);
					break;
				}
			}

		}
		else {
			reportTracking reporter;

			reporter.setID(currentTrackStruture[k].getTrackID());
			reporter.addPath(currentTrackStruture[k].getCenterPoint());
			reporter.addPathRect(currentTrackStruture[k].getROI());
			pathList.push_back(reporter);
		}
	}
	return pathList;
}

void Tracking::showPath(vector<reportTracking> pathList, Mat pathImg)
{
	RNG rng(12345);
	Mat normalized = pathImg.clone();
	vector<Point> pointList;
	vector<Point> pointListTemp;
	for (int i = 0; i < pathList.size(); i++) {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		for (int j = 0; j < pathList[i].getPath().size(); j++) {
			pointList.push_back(pathList[i].getPath()[j]);
			pointListTemp.push_back(pathList[i].getPath()[j]);
			circle(pathImg, pathList[i].getPath()[j], 3, color, -1);

		}
	}
	vector<pair<Point, int>> pairPath;
	Point temp;

	for (int i = 0; i < pointList.size(); i++) {
		int weight = 0;
		Point temp = pointList[i];

		for (int j = 0; j < pointListTemp.size(); j++) {

			if (temp == pointListTemp[j]) {
				weight++;
			}
		}
		if (pairPath.size() == 0) {
			pair<Point, int> it;
			it.first = temp;
			it.second = weight;
			pairPath.push_back(it);
		}
		else {
			bool sameVal = false;
			for (int k = 0; k < pairPath.size(); k++) {
				if (temp == pairPath[k].first) {
					sameVal = true;
				}
			}
			if (!sameVal) {
				pair<Point, int> it;
				it.first = temp;
				it.second = weight;
				pairPath.push_back(it);
			}
		}
	}

	for (int i = 0; i < pairPath.size(); i++) {
		Scalar color = Scalar(0,255,0);
		if (pairPath[i].second > 10) {
			circle(normalized, pairPath[i].first, 3, color, -1);
		}
		}
	

	imshow("Path", pathImg);
	imshow("Normalized Path", normalized);
	waitKey(0);
}

void Tracking::showTrack(vector<reportTracking> pathList, vector<trackStructure> currentTrackStruture, Mat trackImg)
{
	/*vector<int> eraseTrack;
	for (int i = 0; i < pathList.size(); i++) {
		if (pathList[i].getPathRect().size() <= 10) {
			continue;
		}

	}*/


	for (int i = 0; i < currentTrackStruture.size(); i++) {

		rectangle(trackImg, currentTrackStruture[i].getROI(), Scalar(255, 0, 0), 2, 1);
		putText(trackImg, "id:" + to_string(currentTrackStruture[i].getTrackID()), currentTrackStruture[i].getROI().tl(), 1, 2, Scalar(255, 0, 255), 2);
		imshow("Track", trackImg);

	}

}