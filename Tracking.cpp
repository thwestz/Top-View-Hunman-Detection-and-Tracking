#include "Tracking.h"
#include "samples_utility.cpp"

MultiTracker Tracking::tracking_API(Mat frame, vector<Rect> ROIs, MultiTracker currentTrackers) {

	vector<Ptr<Tracker> > algorithms;
	MultiTracker temptrackers;
	temptrackers.clear();
	temptrackers = currentTrackers;
	currentTrackers.clear();
	vector<Rect2d> objects;
	for (size_t i = 0; i < ROIs.size(); i++)
	{
		if (temptrackers.getObjects().size() == 0) {
			algorithms.push_back(createTrackerByName("KCF"));
			objects.push_back(ROIs[i]);
			currentTrackers.add(algorithms[i], frame, objects[i]);
			continue;
		}

		/*int sx = ROIs[i].tl().x;
		int sy = ROIs[i].tl().y;
		int ex = ROIs[i].br().x - 1;
		int ey = ROIs[i].br().y - 1;
		int cx = (sx + ex) / 2;
		int cy = (sy + ey) / 2;
		Point centerROI(cx, cy);*/
		for (size_t j = 0; j < temptrackers.getObjects().size(); j++)
		{
			//if(temptrackers.getObjects() <= )
			/*int kx = temptrackers.getObjects()[j].tl().x;
			int ky = temptrackers.getObjects()[j].tl().y;
			int jx = temptrackers.getObjects()[j].br().x - 1;
			int jy = temptrackers.getObjects()[j].br().y - 1;
			int vx = (kx + jx) / 2;
			int vy = (ky + jy) / 2;
			Point centerNew(vx, vy);
			if ( (centerNew.x > centerROI.x + 30 || centerNew.x < centerROI.x - 30 ) 
				 && (centerNew.y > centerROI.y + 30 || centerNew.y < centerROI.y -30) ) {
				//continue;
				algorithms.push_back(createTrackerByName("KCF"));
				objects.push_back(ROIs[i]);
				currentTrackers.add(algorithms[i], frame, objects[i]);
			}*/
			/*else {
				continue;*/
				/*algorithms.push_back(createTrackerByName("KCF"));
				objects.push_back(ROIs[i]);
				currentTrackers.add(algorithms[i], frame, objects[i]);*/
			//}
		}
	}


	currentTrackers.update(frame);


	for (unsigned i = 0; i < currentTrackers.getObjects().size(); i++) {
		rectangle(frame, currentTrackers.getObjects()[i], Scalar(255, 0, 0), 2, 1);

	}
	resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
	imshow("tracker", frame);


	return currentTrackers;
}