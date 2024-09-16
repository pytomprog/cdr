#pragma once

#ifndef CDR_ARUCODETECTOR_HPP
#define CDR_ARUCODETECTOR_HPP

#include <map>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <spdlog/spdlog.h>
#include "SpdlogUtils.hpp"

#include "camera/Camera.hpp"
#include "Profiler.hpp"
#include "aruco/ArucoUtils.hpp"

class ArucoDetector {
public:
	Camera& m_camera;
	Profiler& m_profiler;

	cv::aruco::Dictionary m_dictionary;
	cv::aruco::DetectorParameters m_cvDetectorParams;
	cv::aruco::ArucoDetector m_cvDetector;

	std::vector<std::vector<cv::Point2f>> m_markersCorners, m_rejectedCandidates;
	std::vector<int> m_markersId;
	std::map<int, cv::Vec3f> m_rvecs, m_tvecs;
	std::vector<int> m_tableMarkersId;
	cv::Vec3f m_tableRvec, m_tableTvec;
	bool m_tableAlreadyDetected;

	int m_focusMarkerId;

	cv::Mat m_croppedFrame;
	cv::Rect m_cropRectangle;
	cv::Rect m_futureCropRectangle;
	
	ArucoDetector(Camera& camera, Profiler& profiler, int focusMarkerId);
	void getMarkers3dPosition(cv::Mat& inputFrame);
	void drawResults(cv::Mat& inputFrame);
};

#endif // CDR_ARUCODETECTOR_HPP
