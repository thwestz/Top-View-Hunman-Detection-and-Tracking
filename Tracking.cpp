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


	for ( int z = 0; z < ROIs.size(); z++) {
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
		newTrack.setTrackID(i+1);
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

void Tracking::showPath(vector<reportTracking> pathList, Mat pathImg, int fps)
{
	RNG rng(12345);
	Mat normalized = pathImg.clone();
	vector<Point> pointList;
	vector<Point> pointListTemp;
	string filePath;
	ofstream outputfile;
	outputfile.open("D:/Senior_Project/result.txt", std::ios_base::app);
	if (!outputfile.is_open()) {

		std::ofstream newFile("D:/Senior_Project/result.txt");
		newFile.close();
		outputfile.open("D:/Senior_Project/result.txt", std::ios_base::app);
	}
	for (int i = 0; i < pathList.size(); i++) {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		for (int j = 0; j < pathList[i].getPath().size(); j++) {
			pointList.push_back(pathList[i].getPath()[j]);
			pointListTemp.push_back(pathList[i].getPath()[j]);
			circle(pathImg, pathList[i].getPath()[j], 3, color, -1);
		}
			outputfile << "Point : " << pathList[i].getPath() << "\n" << " ID : " << pathList[i].getID() << " Color : " << color.val[0] << "," << color.val[1] << "," << color.val[2] << "\n";
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
		Scalar color = Scalar(0, 255, 0);

		printf_s("%s %d %s", "ID : ", pairPath[i].second, "\n");

		printf_s("%d %d %d %s", pairPath[i].first.x, pairPath[i].first.y, pairPath[i].second, "\n");
		if (pairPath[i].second > 5) {
			circle(normalized, pairPath[i].first, 3, color, -1);
		}
	}

	outputfile.close();
	imshow("Path", pathImg);
	imshow("Normalized Path", normalized);
	waitKey(0);
}


void Tracking::showTrack(vector<reportTracking> pathList, vector<trackStructure> currentTrackStruture, Mat trackImg, vector<pair<int, int>>chk_failure_track, int fps)
{
	int cnt_frame;
	if (chk_failure_track.size() == 0) {
		for (int i = 1; i < currentTrackStruture.size(); i++) {

			rectangle(trackImg, currentTrackStruture[i].getROI(), Scalar(255, 0, 0), 2, 1);
			putText(trackImg, "id:" + to_string(currentTrackStruture[i].getTrackID()), currentTrackStruture[i].getROI().tl(), 1, 2, Scalar(255, 0, 255), 2);
			//recorder.write(trackImg);
			imshow("Track", trackImg);

		}
	}
	else
	{

		for (int i = 1; i < currentTrackStruture.size(); i++)
		{
			for (int j = 0; j < chk_failure_track.size(); j++)

			{
				if (i == chk_failure_track[j].first)
				{
					cnt_frame = chk_failure_track[j].second;
				}
			}
			if (cnt_frame < fps * 5)
			{
				rectangle(trackImg, currentTrackStruture[i].getROI(), Scalar(255, 0, 0), 2, 1);
				putText(trackImg, "id:" + to_string(currentTrackStruture[i].getTrackID() - 1), currentTrackStruture[i].getROI().tl(), 1, 2, Scalar(255, 0, 255), 2);

				//recorder.write(trackImg);

				imshow("Track", trackImg);

			}

		}
	}
}

vector<pair<int, int>> Tracking::cnt_failure_tracking(MultiTracker currentTracker, vector<Rect2d> ROIs, vector<pair<int, int>> vec_chk_track) {
	pair<int, int> chk_failure_track = make_pair(0, 0);
	bool flag = false;
	bool no_more_add = false;
	for (int i = 0; i < currentTracker.getObjects().size(); i++)
	{
		for (int j = 0; j < ROIs.size(); j++)
		{
			Rect2d overlap = currentTracker.getObjects()[i] & ROIs[j];
			Rect2d totalSize = currentTracker.getObjects()[i] | ROIs[j];


			if (((overlap.area() * 100.00) / totalSize.area() > 45.00))
			{
				if (vec_chk_track.size() > 0)
				{
					for (int l = 0; l < vec_chk_track.size(); l++)
					{
						if (i == vec_chk_track[l].first)
						{
							vec_chk_track[l].second = 0;
						}
						else if (i == vec_chk_track[l].first && ((overlap.area() * 100.00) / totalSize.area() > 85.00))
						{
							vec_chk_track[l].second = 0;
						}
					}
				}
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
