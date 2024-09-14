#include "aruco/ArucoDetector.hpp"

ArucoDetector::ArucoDetector(Camera& camera, Profiler& profiler, int focusMarkerId) : m_camera(camera), m_profiler(profiler) {
	m_dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
	m_cvDetectorParams = cv::aruco::DetectorParameters();
	m_cvDetectorParams.useAruco3Detection = true;
	m_cvDetector = cv::aruco::ArucoDetector(m_dictionary, m_cvDetectorParams);

	m_focusMarkerId = focusMarkerId;

	m_futureCropRectangle = cv::Rect(0, 0, camera.m_width, camera.m_height);
}

void ArucoDetector::getMarkers3dPosition(cv::Mat& inputFrame) {
	m_cropRectangle = m_futureCropRectangle;
	m_croppedFrame = inputFrame(m_cropRectangle);
	m_profiler.updateTimepoint(1);

	m_cvDetector.detectMarkers(m_croppedFrame, m_markersCorners, m_markersId, m_rejectedCandidates);
	for (std::vector<cv::Point2f>& markerCorners : m_markersCorners) {
		for (cv::Point2f& markerCorner : markerCorners)
			markerCorner += cv::Point2f(m_cropRectangle.x, m_cropRectangle.y);
	}

	m_profiler.updateTimepoint(2);

	if (m_markersId.size() > 0) {
		// Improve future crop rectangle
		//  TODO: Take the good marker and not necessarly the first one
		improveFutureCropRectangle(inputFrame, m_markersCorners[0], m_futureCropRectangle);
	} else {
		// Reset future crop rectangle
		m_futureCropRectangle = { 0, 0, inputFrame.cols, inputFrame.rows };
	}
	m_profiler.updateTimepoint(3);

	for (size_t i = 0; i < m_markersId.size(); i++) {
		int markerId = m_markersId[i];
		// if useExtrinsicGuess is true, cv::solvePnP begins with provided values for rvec and tvec 
		// so cv::solvePnP begins with the previous values if they are not changed between
		bool useExtrinsicGuess = m_rvecs.contains(markerId) && m_tvecs.contains(markerId); // If true, we assume that tves also contains markerId
		cv::solvePnP(marker3dPoints(markerId), m_markersCorners[i], m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, m_rvecs[markerId], m_tvecs[markerId], useExtrinsicGuess, cv::SOLVEPNP_ITERATIVE); //cv::SOLVEPNP_IPPE_SQUARE);
	}
}

void ArucoDetector::drawResults(cv::Mat& inputFrame) {
	cv::rectangle(inputFrame, m_cropRectangle, cv::Scalar(255, 0, 0));
	cv::aruco::drawDetectedMarkers(inputFrame, m_markersCorners, m_markersId);
	for (size_t i = 0; i < m_markersId.size(); i++) {
		int markerId = m_markersId[i];
		cv::drawFrameAxes(inputFrame, m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, m_rvecs[markerId], m_tvecs[markerId], markerLength(markerId) * 1.5f, 2);
		
		cv::Mat tableProjectedPoints; 
		cv::Mat tableUnprojectedPoints = table3dPoints();
		cv::projectPoints(tableUnprojectedPoints, m_rvecs[markerId], m_tvecs[markerId], m_camera.m_intrinsicMatrix, m_camera.m_distortionCoefficients, tableProjectedPoints);
		tableProjectedPoints.convertTo(tableProjectedPoints, CV_32S);
		cv::polylines(inputFrame, tableProjectedPoints, true, cv::Scalar(255, 255, 0));
	}
}
