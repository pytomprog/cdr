#pragma once

#ifndef CDR_CAMERA_HPP
#define CDR_CAMERA_HPP

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <spdlog/spdlog.h>

class Camera {
public:
	int m_width, m_height;
	cv::Mat m_intrinsicMatrix;
	cv::Mat m_distortionCoefficients;
	
	cv::VideoCapture m_cap;

	Camera(int width, int height);

	int open(std::string source);
	void getImage(cv::Mat& frame);
	void close();
};

#endif // CDR_CAMERA_HPP