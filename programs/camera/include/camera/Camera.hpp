#pragma once

#ifndef CDR_CAMERA_HPP
#define CDR_CAMERA_HPP

#include <iostream>
#include <memory>
#include <thread>
#include <functional>

#ifdef ARM
	#include <libcamera/libcamera.h>
	#include "camera/event_loop.h"
	#include "camera/mapped_framebuffer.h"
#endif //ARM

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <spdlog/spdlog.h>
#include "SpdlogUtils.hpp"


#ifdef ARM
	static std::shared_ptr<libcamera::Camera> libcameraCamera;
	static std::unique_ptr<libcamera::CameraConfiguration> libcameraCameraConfiguration;
	static libcamera::Stream* streamPtr;
	static std::unique_ptr<libcamera::CameraManager> libcameraCameraManager;
	static libcamera::FrameBufferAllocator* allocatorPtr;
	static EventLoop loop;
#endif //ARM
extern std::function<int(cv::Mat)> processFrameFunction;

class Camera {
public:
	int m_width, m_height;
	cv::Mat m_intrinsicMatrix;
	cv::Mat m_distortionCoefficients;
	
	#ifdef ARM
		std::vector<std::unique_ptr<libcamera::Request>> m_startingRequests;
	#else
		cv::VideoCapture m_cap;
	#endif //ARM

	Camera(int width, int height);

	int open(std::string source);
	void run();
	void close();
};

#ifdef ARM
	static void requestComplete(libcamera::Request *request);
	static void processRequest(libcamera::Request *request);
	std::string libcameraCameraName(libcamera::Camera *libcameraCamera);
	void changeControls(libcamera::ControlList& controls);
#endif //ARM

#endif // CDR_CAMERA_HPP
