include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;

class Blob {
public:

	vector<Point> contour;

	Rect boundingRect;

	Point centerPosition;

	double dblDiagonalSize;

	double dblAspectRatio;

	Blob(vector<Point> _contour);

};
