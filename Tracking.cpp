#include "Tracking.h"
#include "samples_utility.cpp"
#include "Math.h"


MultiTracker Tracking::tracking_API(Mat frame, vector<Rect> ROIs, MultiTracker currentTrackers) {
	vector<Ptr<Tracker> > algorithms;
	MultiTracker temptrackers;
	//temptrackers.clear();
	temptrackers = currentTrackers;
	currentTrackers.clear();
	vector<Rect2d> objects;
	for (size_t i = 0; i < ROIs.size(); i++)
	{
		if (temptrackers.getObjects().size() == 0) {
			algorithms.push_back(createTrackerByName("KCF"));
			objects.push_back(ROIs[i]);
			temptrackers.add(algorithms[i], frame, objects[i]);
			continue;
		}
		/*else
		{
			int sx = ROIs[i].tl().x;
			int sy = ROIs[i].tl().y;
			int ex = ROIs[i].br().x - 1;
			int ey = ROIs[i].br().y - 1;
			int cx = (sx + ex) / 2;
			int cy = (sy + ey) / 2;
			Point centerROI(cx, cy);
			for (size_t j = 0; j < temptrackers.getObjects().size(); j++)
			{
				//if(temptrackers.getObjects() <= )
				int kx = temptrackers.getObjects()[j].tl().x;
				int ky = temptrackers.getObjects()[j].tl().y;
				int jx = temptrackers.getObjects()[j].br().x - 1;
				int jy = temptrackers.getObjects()[j].br().y - 1;
				int vx = (kx + jx) / 2;
				int vy = (ky + jy) / 2;
				Point centerNew(vx, vy);
				if (sqrt(pow(centerNew.x-centerROI.x,2)-pow(centerNew.y-centerROI.y,2))<=30) {
					//continue;
					break;
				}
				else
				{
					algorithms.push_back(createTrackerByName("KCF"));
					objects.push_back(ROIs[i]);
					currentTrackers.add(algorithms[i], frame, objects[i]);
				}
			}
		}*/
	}


	temptrackers.update(frame);


	for (unsigned i = 0; i < temptrackers.getObjects().size(); i++) {
		int kx = (temptrackers.getObjects()[i].tl().x);
		int ky = (temptrackers.getObjects()[i].tl().y)/6;
		int jx = (temptrackers.getObjects()[i].br().x - 81);
		int jy = (temptrackers.getObjects()[i].br().y - 81)*1.5;
		int vx = (kx + jx) / 2;
		int vy = (ky + jy) / 2;
		Point centerNew(vx, vy);
		rectangle(frame, temptrackers.getObjects()[i], Scalar(255, 0, 0), 2, 1);
		circle(frame, centerNew,3, Scalar(0, 255, 0), -1);

	}
	resize(frame, frame, Size(1024, 576));
	imshow("tracker", frame);


	return currentTrackers;
}