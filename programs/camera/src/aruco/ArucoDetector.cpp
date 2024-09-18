#include "aruco/ArucoDetector.hpp"

ArucoDetector::ArucoDetector(Camera& camera, Profiler& profiler, Team ownTeam, int focusMarkerId) : m_camera(camera), m_profiler(profiler) {
	m_dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
	m_cvDetectorParams = cv::aruco::DetectorParameters();
	m_cvDetectorParams.useAruco3Detection = true;
	m_cvDetector = cv::aruco::ArucoDetector(m_dictionary, m_cvDetectorParams);
	
	m_tableAlreadyDetected = false;
	m_ownRobotAlreadyDetected = false;

	m_futureCropRectangle = cv::Rect(0, 0, camera.m_width, camera.m_height);

	m_ownTeam = ownTeam;
	m_focusMarkerId = focusMarkerId;
}

void ArucoDetector::getMarkers3dPosition(cv::Mat& inputFrame) {
	static int iteration = 0;

	m_markersId.clear(); // Already done in m_cvDetector.detectMarkers()
	m_tableMarkersId.clear(); // But this line is necessary
	m_ownRobotDetected = false;

	m_cropRectangle = m_futureCropRectangle;
	m_croppedFrame = inputFrame(m_cropRectangle);
	m_profiler.updateTimepoint(1);

	std::vector<cv::Point2f> tableMarkersCorners;
	std::vector<cv::Point2f> ownRobotMarkerCorners;
	m_cvDetector.detectMarkers(m_croppedFrame, m_markersCorners, m_markersId, m_rejectedCandidates);
	for (size_t i = 0; i < m_markersId.size(); i++) {
		int markerId = m_markersId[i];
		std::vector<cv::Point2f>& markerCorners = m_markersCorners[i];
		for (cv::Point2f& markerCorner : markerCorners)
			markerCorner += cv::Point2f(m_cropRectangle.x, m_cropRectangle.y);

		if ((markerId >= 1 && markerId <= 5 && m_ownTeam == BLUE_TEAM) || (markerId >= 6 && markerId <= 10 && m_ownTeam == YELLOW_TEAM)) {
			ownRobotMarkerCorners = markerCorners;
			m_ownRobotDetected = true;
		} else if ((markerId >= 1 && markerId <= 5 && m_ownTeam != BLUE_TEAM) || (markerId >= 6 && markerId <= 10 && m_ownTeam != YELLOW_TEAM)) {
			//TODO: Adverse robot detection code
		} else if (markerId >= 20 && markerId <= 23) {
			m_tableMarkersId.push_back(markerId);
			tableMarkersCorners.insert(tableMarkersCorners.end(), markerCorners.begin(), markerCorners.end());
		}
	}

	m_profiler.updateTimepoint(2);

	/*if (m_markersId.size() > 0) {
		// Improve future crop rectangle
		//  TODO: Take the good marker and not necessarly the first one
		improveFutureCropRectangle(inputFrame, m_markersCorners[0], m_futureCropRectangle);
	} else {
		// Reset future crop rectangle
		m_futureCropRectangle = { 0, 0, inputFrame.cols, inputFrame.rows };
	}*/
	m_profiler.updateTimepoint(3);

	for (size_t i = 0; i < m_markersId.size(); i++) {
		int markerId = m_markersId[i];
		// if useExtrinsicGuess is true, cv::solvePnP begins with provided values for rvec and tvec 
		// so cv::solvePnP begins with the previous values if they are not changed between
		bool useExtrinsicGuess = m_rvecs.contains(markerId) && m_tvecs.contains(markerId); // If true, we assume that tves also contains markerId
		cv::solvePnP(marker3dPoints(markerId), m_markersCorners[i], m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, m_rvecs[markerId], m_tvecs[markerId], useExtrinsicGuess, cv::SOLVEPNP_ITERATIVE); //cv::SOLVEPNP_IPPE_SQUARE);
	}

	if (m_tableMarkersId.size() > 0) {
		bool useExtrinsicGuess = m_tableAlreadyDetected;
		cv::Mat tableMarkersPoints = tableMarkers3dPoints(m_tableMarkersId);
		cv::solvePnP(tableMarkersPoints, tableMarkersCorners, m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, m_tableRvec, m_tableTvec, useExtrinsicGuess, cv::SOLVEPNP_ITERATIVE); //cv::SOLVEPNP_IPPE_SQUARE);
		inversePerspective(m_tableRvec, m_tableTvec, m_tableInvRvec, m_tableInvTvec);

		m_tableAlreadyDetected = true;
	}

	if (m_ownRobotDetected && iteration >= 10) {
		bool useExtrinsicGuess = m_ownRobotAlreadyDetected;
		cv::solvePnP(marker3dPoints(1), ownRobotMarkerCorners, m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, m_ownRobotRvecCameraFrame, m_ownRobotTvecCameraFrame, useExtrinsicGuess, cv::SOLVEPNP_ITERATIVE); //cv::SOLVEPNP_IPPE_SQUARE);
		
		cv::composeRT(m_ownRobotRvecCameraFrame, m_ownRobotTvecCameraFrame, m_tableInvRvec, m_tableInvTvec, m_ownRobotRvecTableFrame, m_ownRobotTvecTableFrame);

		spdlog::info("Robot position: {}, rotation: {} (in table frame)", m_ownRobotTvecTableFrame, m_ownRobotRvecTableFrame);

		m_ownRobotAlreadyDetected = true;
	}

	iteration++;
}

void ArucoDetector::drawResults(cv::Mat& inputFrame) {
	cv::rectangle(inputFrame, m_cropRectangle, cv::Scalar(255, 0, 0));
	cv::aruco::drawDetectedMarkers(inputFrame, m_markersCorners, m_markersId);
	for (size_t i = 0; i < m_markersId.size(); i++) {
		int markerId = m_markersId[i];
		cv::drawFrameAxes(inputFrame, m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, m_rvecs[markerId], m_tvecs[markerId], markerLength(markerId) * 1.5f, 2);
	}

	if (m_tableMarkersId.size() > 0) {
		cv::Mat tableProjectedPoints;
		cv::Mat tableUnprojectedPoints = table3dPoints();
		cv::projectPoints(tableUnprojectedPoints, m_tableRvec, m_tableTvec, m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, tableProjectedPoints);
		tableProjectedPoints.convertTo(tableProjectedPoints, CV_32S);
		cv::polylines(inputFrame, tableProjectedPoints, true, cv::Scalar(255, 0, 255));
		cv::drawFrameAxes(inputFrame, m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, m_tableRvec, m_tableTvec, 100, 2);
	}

	if (m_ownRobotDetected) {
		cv::drawFrameAxes(inputFrame, m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, m_ownRobotRvecCameraFrame, m_ownRobotTvecCameraFrame, 100, 2);
	}
}
