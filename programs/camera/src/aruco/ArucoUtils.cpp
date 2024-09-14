#include "aruco/ArucoUtils.hpp"

void improveFutureCropRectangle(cv::Mat& inputFrame, std::vector<cv::Point2f>& markerCorners, cv::Rect& futureCropRectangle) {
	int minXCorner = std::min({ markerCorners[0].x, markerCorners[1].x, markerCorners[2].x, markerCorners[3].x });
	int maxXCorner = std::max({ markerCorners[0].x, markerCorners[1].x, markerCorners[2].x, markerCorners[3].x });
	int minYCorner = std::min({ markerCorners[0].y, markerCorners[1].y, markerCorners[2].y, markerCorners[3].y });
	int maxYCorner = std::max({ markerCorners[0].y, markerCorners[1].y, markerCorners[2].y, markerCorners[3].y });
	//std::cout << "minXCorner: " << minXCorner << ", maxXCorner: " << maxXCorner << ", minYCorner: " << minYCorner << ", maxYCorner: " << maxYCorner << std::endl;
	int x = 2 * minXCorner - maxXCorner;
	int y = 2 * minYCorner - maxYCorner;
	int width = 3 * (maxXCorner - minXCorner);
	int height = 3 * (maxYCorner - minYCorner);
	int correctedX = std::clamp(x, 0, inputFrame.cols);
	int correctedY = std::clamp(y, 0, inputFrame.rows);
	int correctedWidth = std::clamp(width - (correctedX - x), 0, inputFrame.cols - correctedX);
	int correctedHeight = std::clamp(height - (correctedY - y), 0, inputFrame.rows - correctedY);
	//std::cout << "x: " << x << ", y: " << y << ", width: " << width << ", height: " << height << std::endl;
	//std::cout << "correctedX: " << correctedX << ", correctedY: " << correctedY << ", correctedWidth: " << correctedWidth << ", correctedHeight: " << correctedHeight << std::endl;
	futureCropRectangle = { correctedX, correctedY, correctedWidth, correctedHeight };
}

float markerLength(int markerId) {
	if (markerId >= 1 && markerId <= 10) {
		return 70.f;
	} else if (markerId >= 20 && markerId <= 23) {
		return 100.f;
	}
	return 50.f;
}

cv::Mat marker3dPoints(int markerId) {
	int length = markerLength(markerId);
	cv::Mat points = cv::Mat(4, 1, CV_32FC3);
	points.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-length / 2.f, length / 2.f, 0);
	points.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(length / 2.f, length / 2.f, 0);
	points.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(length / 2.f, -length / 2.f, 0);
	points.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-length / 2.f, -length / 2.f, 0);
	return points;
}

cv::Mat table3dPoints() {
	//TEST: Not to the real dimensions (500x500 actually) and drawn for every detected marker and centered on it
	//TODO: Program final version
	
	cv::Mat points = cv::Mat(4, 1, CV_32FC3);
	points.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-250, 250, 0);
	points.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(250, 250, 0);
	points.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(250, -250, 0);
	points.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-250, -250, 0);
	return points;
}
