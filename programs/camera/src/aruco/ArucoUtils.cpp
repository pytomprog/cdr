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