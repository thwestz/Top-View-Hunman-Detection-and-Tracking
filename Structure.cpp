#include "Structure.h"

void Structure::setTracking_id(size_t element, unsigned total_rois, int id) {
	for (unsigned i = element; i < total_rois; i++)
	{
		tracking_id.push_back(id);
	}
}
/*
Class Track{
id;
status;
timestamp;
}

track.id;

*/
vector<int> Structure::getTracking_id() {
	return tracking_id;
}

void Structure::setStatus(String status) {
	tracking_status = status;
}

String Structure::getStatus() {
	return tracking_status;
}