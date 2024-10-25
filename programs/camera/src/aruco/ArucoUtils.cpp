#include "aruco/ArucoUtils.hpp"

float optimizationCoefficient = 1.f; // 1.f by default, lower is faster, higher it's more robust
void improveFutureCropRectangle(cv::Mat& inputFrame, std::vector<cv::Point2f>& markerCorners, cv::Rect& futureCropRectangle) {
	int minXCorner = std::min({ markerCorners[0].x, markerCorners[1].x, markerCorners[2].x, markerCorners[3].x });
	int maxXCorner = std::max({ markerCorners[0].x, markerCorners[1].x, markerCorners[2].x, markerCorners[3].x });
	int minYCorner = std::min({ markerCorners[0].y, markerCorners[1].y, markerCorners[2].y, markerCorners[3].y });
	int maxYCorner = std::max({ markerCorners[0].y, markerCorners[1].y, markerCorners[2].y, markerCorners[3].y });
	//std::cout << "minXCorner: " << minXCorner << ", maxXCorner: " << maxXCorner << ", minYCorner: " << minYCorner << ", maxYCorner: " << maxYCorner << std::endl;
	int x = (1 + optimizationCoefficient) * minXCorner - optimizationCoefficient * maxXCorner;
	int y = (1 + optimizationCoefficient) * minYCorner - optimizationCoefficient * maxYCorner;
	int width = (1 + 2 * optimizationCoefficient) * (maxXCorner - minXCorner);
	int height = (1 + 2 * optimizationCoefficient) * (maxYCorner - minYCorner);
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

cv::Mat tableMarkers3dPoints(std::vector<int> markersId) {
	cv::Mat points = cv::Mat(4*markersId.size(), 1, CV_32FC3);
	for (size_t i = 0; i < markersId.size(); i++) {
		cv::Mat currentPoints = marker3dPoints(markersId[i]);

		cv::Vec3f basePosition;
		switch (markersId[i]) {
		case 20:
			basePosition = cv::Vec3f(750.f, 1500.f, 0.f);
			break;
		case 21:
			basePosition = cv::Vec3f(2250.f, 1500.f, 0.f);
			break;
		case 22:
			basePosition = cv::Vec3f(750.f, 500.f, 0.f);
			break;
		case 23:
			basePosition = cv::Vec3f(2250.f, 500.f, 0.f);
			break;
			
		// WARNING: THIS IS ONLY FOR VILLAGE DES SCIENCES
		/*case 22:
			basePosition = cv::Vec3f(1000.f, 1000.f, 0.f);
			break;*/
		default:
			break;
		}

		for (int j = 0; j < 4; j++)
			points.ptr<cv::Vec3f>(0)[4 * i + j] = basePosition + currentPoints.ptr<cv::Vec3f>(0)[j];
	}
	return points;
}

cv::Mat table3dPoints() {
	cv::Mat points = cv::Mat(16, 1, CV_32FC3);
	points.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(0.f, 0.f, 0.f);
	points.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(3000.f, 0.f, 0.f);
	points.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(3000.f, 2000.f, 0.f);
	points.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(0.f, 2000.f, 0.f);
	points.ptr<cv::Vec3f>(0)[4] = cv::Vec3f(0.f, 0.f, 0.f);
	points.ptr<cv::Vec3f>(0)[5] = cv::Vec3f(0.f, 0.f, 70.f);
	points.ptr<cv::Vec3f>(0)[6] = cv::Vec3f(3000.f, 0.f, 70.f);
	points.ptr<cv::Vec3f>(0)[7] = cv::Vec3f(3000.f, 0.f, 0.f);
	points.ptr<cv::Vec3f>(0)[8] = cv::Vec3f(3000.f, 0.f, 70.f);
	points.ptr<cv::Vec3f>(0)[9] = cv::Vec3f(3000.f, 2000.f, 70.f);
	points.ptr<cv::Vec3f>(0)[10] = cv::Vec3f(3000.f, 2000.f, 0.f);
	points.ptr<cv::Vec3f>(0)[11] = cv::Vec3f(3000.f, 2000.f, 70.f);
	points.ptr<cv::Vec3f>(0)[12] = cv::Vec3f(0.f, 2000.f, 70.f);
	points.ptr<cv::Vec3f>(0)[13] = cv::Vec3f(0.f, 2000.f, 0.f);
	points.ptr<cv::Vec3f>(0)[14] = cv::Vec3f(0.f, 2000.f, 70.f);
	points.ptr<cv::Vec3f>(0)[15] = cv::Vec3f(0.f, 0.f, 70.f);
	
	// WARNING: THIS IS ONLY FOR VILLAGE DES SCIENCES
	/*cv::Mat points = cv::Mat(16, 1, CV_32FC3);
	points.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(0.f, 0.f, 0.f);
	points.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(2000.f, 0.f, 0.f);
	points.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(2000.f, 2000.f, 0.f);
	points.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(0.f, 2000.f, 0.f);
	points.ptr<cv::Vec3f>(0)[4] = cv::Vec3f(0.f, 0.f, 0.f);
	points.ptr<cv::Vec3f>(0)[5] = cv::Vec3f(0.f, 0.f, 70.f);
	points.ptr<cv::Vec3f>(0)[6] = cv::Vec3f(2000.f, 0.f, 70.f);
	points.ptr<cv::Vec3f>(0)[7] = cv::Vec3f(2000.f, 0.f, 0.f);
	points.ptr<cv::Vec3f>(0)[8] = cv::Vec3f(2000.f, 0.f, 70.f);
	points.ptr<cv::Vec3f>(0)[9] = cv::Vec3f(2000.f, 2000.f, 70.f);
	points.ptr<cv::Vec3f>(0)[10] = cv::Vec3f(2000.f, 2000.f, 0.f);
	points.ptr<cv::Vec3f>(0)[11] = cv::Vec3f(2000.f, 2000.f, 70.f);
	points.ptr<cv::Vec3f>(0)[12] = cv::Vec3f(0.f, 2000.f, 70.f);
	points.ptr<cv::Vec3f>(0)[13] = cv::Vec3f(0.f, 2000.f, 0.f);
	points.ptr<cv::Vec3f>(0)[14] = cv::Vec3f(0.f, 2000.f, 70.f);
	points.ptr<cv::Vec3f>(0)[15] = cv::Vec3f(0.f, 0.f, 70.f);*/
	
	return points;
}

void inversePerspective(cv::Vec3f& rvec, cv::Vec3f& tvec, cv::Vec3f& invRvec, cv::Vec3f& invTvec) {
	cv::Mat R;
	cv::Rodrigues(rvec, R);
	R = R.t();
	invTvec = -cv::Vec3f(R.at<float>(0, 0) * tvec[0] + R.at<float>(0, 1) * tvec[1] + R.at<float>(0, 2) * tvec[2],
		R.at<float>(1, 0)* tvec[0] + R.at<float>(1, 1) * tvec[1] + R.at<float>(1, 2) * tvec[2],
		R.at<float>(2, 0)* tvec[0] + R.at<float>(2, 1) * tvec[1] + R.at<float>(2, 2) * tvec[2]);
	cv::Rodrigues(R, invRvec);
}
