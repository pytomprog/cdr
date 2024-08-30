#include "Camera.hpp"

Camera::Camera(int width = 1920, int height = 1080) {
	m_width = width;
	m_height = height;

	m_intrinsicMatrix = cv::Mat(3, 3, CV_32FC1, new float[] { 1300.f, 0.f, m_width / 2.f, 0.f, 1300.f, m_height / 2.f, 0.f, 0.f, 1.f });
	m_distortionCoefficients = cv::Mat(1, 5, CV_32FC1, new float[] { 0.f, 0.f, 0.f, 0.f, 0.f}); // Maybe 5 rows and 1 column
}


int Camera::open(std::string source) {
	if (std::find_if(source.begin(), source.end(), [](unsigned char c) { return !std::isdigit(c); }) == source.end()) {
		m_cap.open(atoi(source.c_str()));
	} else {
		m_cap.open(source);
	}
	
	if (!m_cap.isOpened()) {
		spdlog::error("No video stream detected");
		return EXIT_FAILURE;
	}
	m_cap.set(cv::CAP_PROP_FRAME_WIDTH, m_width);
	m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, m_height);
	cv::Mat testFrame;
	m_cap >> testFrame;
	if (testFrame.empty()) {
		spdlog::error("Test frame is empty");
		return EXIT_FAILURE;
	}
	m_width = testFrame.cols; // In case of resolution slightly changed
	m_height = testFrame.rows;
	return EXIT_SUCCESS;
}

void Camera::getImage(cv::Mat& frame) {
	m_cap >> frame;
}

void Camera::close() {
	if (m_cap.isOpened())
		m_cap.release();
}
