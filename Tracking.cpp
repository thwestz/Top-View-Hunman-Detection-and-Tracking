#include "Tracking.h"

MultiTracker Tracking::adaptMultiTracker(Mat frame, vector<Rect2d> ROIs, MultiTracker currentTrackers,int currentFrame) {
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

vector<reportTracking> Tracking::manageReport(vector<trackStructure> currentTrackStruture, vector<reportTracking> pathList)
{

	for (int k = 0; k < currentTrackStruture.size(); k++) {
		if (pathList.size() > 0) {
			for (int i = 0; i < pathList.size(); i++) {

				if (currentTrackStruture[k].getTrackID() == pathList[i].getID()) {
					pathList[i].addPath(currentTrackStruture[k].getCenterPoint());
					break;
				}

				if (currentTrackStruture[k].getTrackID() != pathList[i].getID() &&
					currentTrackStruture[k].getTrackID() > pathList.size()) {
					reportTracking reporter;

					reporter.setID(currentTrackStruture[k].getTrackID());
					reporter.addPath(currentTrackStruture[k].getCenterPoint());
					pathList.push_back(reporter);
					break;
				}
			}

		}
		else {
			reportTracking reporter;

			reporter.setID(currentTrackStruture[k].getTrackID());
			reporter.addPath(currentTrackStruture[k].getCenterPoint());
			pathList.push_back(reporter);
		}
	}
	return pathList;
}

void Tracking::showPath(vector<reportTracking> pathList,Mat pathImg)
{
	for (int i = 0; i < pathList.size(); i++) {

		for (int j = 0; j < pathList[i].getPath().size(); j++) {
			circle(pathImg, pathList[i].getPath()[j], 3, Scalar(i * 200, 0, 0), -1);

		}

	}
	imshow("Path", pathImg);
	waitKey(0);
}

vector<pair<int, int>> Tracking::cnt_failure_tracking(MultiTracker currentTracker, vector<Rect2d> ROIs, vector<pair<int, int>> vec_chk_track) {
	pair<int, int> chk_failure_track = make_pair(0,0);
	bool flag = false;
	bool no_more_add = false;
	for (int i = 0; i < currentTracker.getObjects().size(); i++) 
	{
		for (int j = 0; j < ROIs.size(); j++) 
		{
			if ((currentTracker.getObjects()[i] & ROIs[j]).area() > 0)
			{
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			if (vec_chk_track.size() == 0)//first time with out tracking failure data
			{
				chk_failure_track = make_pair(i, 1); //make pair with track[i] and cnt when track[i] failure
				vec_chk_track.push_back(chk_failure_track);
				printf_s("frame_0 %d cnt %d ", vec_chk_track[i].first, vec_chk_track[i].second);
				break;
			}
			//this part is bugging
			for (int k = 0; k < vec_chk_track.size(); k++) {
				if (i == vec_chk_track[k].first)
				{
					vec_chk_track[k].second += 1;
					no_more_add = true;
				}
			}
			if (no_more_add == false)
			{
				chk_failure_track = make_pair(i, 1);
				vec_chk_track.push_back(chk_failure_track);
			}

		}
		no_more_add = false;
		flag = false;
	}
	for (int i = 0; i < vec_chk_track.size(); i++)
	{
		printf_s("track_id %d cnt %d round %d \n", vec_chk_track[i].first, vec_chk_track[i].second, i);
		//waitKey(0);
	}
	return vec_chk_track;
}
