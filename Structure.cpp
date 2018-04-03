#include "Structure.h"

void Structure::setTracking_id(int id) {
	tracking_id = id;

}

int Structure::getTracking_id() {
	return tracking_id;
}

void Structure::setStatus(String status) {
	tracking_status = status;
}

String Structure::getStatus() {
	return tracking_status;
}