#include "Structure.h"

void Structure::setTracking_id(int element,size_t total_rois, int id) {
	for (size_t i = element; i < total_rois; i++)
	{
		tracking_id[i] = id;
	}
}

vector<int> Structure::getTracking_id() {
	return tracking_id;
}

void Structure::setStatus(String status) {
	tracking_status = status;
}

String Structure::getStatus() {
	return tracking_status;
}