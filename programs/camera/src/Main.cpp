#include "Main.hpp"

int main() {
	Main m;
	return m.run();
}

Main::Main() {}

int Main::run() {
	spdlog::set_level(logLevel);

	spdlog::debug("OpenCV infos: {}", cv::getBuildInformation().c_str());

	#ifdef PLOT_ENABLED
		m_dataPlotter.addFigure("Marker position", { "tx", "ty", "tz" }, PLOT);
		m_dataPlotter.addFigure("Marker rotation", { "rx", "ry", "rz" }, PLOT);
		m_dataPlotter.addFigure("Marker position histogram", { "tx", "ty", "tz" }, HIST);
		m_dataPlotter.addFigure("Marker rotation histogram", { "rx", "ry", "rz" }, HIST);
		//m_dataPlotter.addFigure("Corners x position", { "0", "1", "2", "3" }, PLOT);
		//m_dataPlotter.addFigure("Corners y position", { "0", "1", "2", "3" }, PLOT);
		m_dataPlotter.addFigure("Table position", { "tx", "ty", "tz" }, PLOT);
		m_dataPlotter.addFigure("Table rotation", { "rx", "ry", "rz" }, PLOT);
		m_dataPlotter.addFigure("Own robot position", { "tx", "ty", "tz" }, PLOT);
		m_dataPlotter.addFigure("Own robot rotation", { "rx", "ry", "rz" }, PLOT);
	#endif // PLOT_ENABLED
	
	processFrameFunction = [this](cv::Mat inputFrame) {return this->step(inputFrame);};
	m_startingTick = cv::getTickCount();
	m_camera.open(cameraSource);
	
	spdlog::info("Camera opened, starting computing robots position...");	
	m_camera.run();
	
	m_camera.close();
	spdlog::info("Camera closed");

	float totalTime = (cv::getTickCount() - m_startingTick) / cv::getTickFrequency();
	spdlog::info("Total iterations {} over {} seconds at a mean rate of {} Hz", m_iteration, totalTime, m_iteration / totalTime);

	#ifdef PLOT_ENABLED
		m_dataPlotter.displayFigures();
	#endif // PLOT_ENABLED

	return EXIT_SUCCESS;
}

int Main::step(cv::Mat inputFrame) {
	if (inputFrame.empty())
		return EXIT_FAILURE;
	
	#ifndef CVWINDOW_ENABLED
		if ((cv::getTickCount() - m_startingTick) / cv::getTickFrequency() > m_timeout) {
			return 2; //TODO: create EXIT_STOP with define and use it
		}
	#endif //CVWINDOW_ENABLED
	
	m_profiler.updateTimepoint(0);
	spdlog::debug("Iteration {}:", m_iteration);

	m_arucoDetector.getMarkers3dPosition(inputFrame);
	m_profiler.updateTimepoint(4);

	spdlog::debug("inputFrame: cols: {}, rows: {}, channels: {}", inputFrame.cols, inputFrame.rows, inputFrame.channels());
	spdlog::debug("croppedFrame: cols: {}, rows: {}, channels: {}", m_arucoDetector.m_croppedFrame.cols, m_arucoDetector.m_croppedFrame.rows, m_arucoDetector.m_croppedFrame.channels());
	spdlog::debug("Makers detected: {}:", m_arucoDetector.m_markersId.size());
	for (int i = 0; i < m_arucoDetector.m_markersId.size(); i++) {
		int markerId = m_arucoDetector.m_markersId[i];
		spdlog::debug("\t- {}:", markerId);
		spdlog::debug("\t\tPosition: {}", m_arucoDetector.m_tvecs[markerId]);
		spdlog::debug("\t\tRotation: {}", m_arucoDetector.m_rvecs[markerId]);
		#ifdef PLOT_ENABLED
			if (markerId == m_arucoDetector.m_focusMarkerId) {
				//m_dataPlotter.m_figures["Corners x position"].addData({ m_arucoDetector.m_markersCorners[i][0].x, m_arucoDetector.m_markersCorners[i][1].x, m_arucoDetector.m_markersCorners[i][2].x, m_arucoDetector.m_markersCorners[i][3].x });
				//m_dataPlotter.m_figures["Corners y position"].addData({ m_arucoDetector.m_markersCorners[i][0].y, m_arucoDetector.m_markersCorners[i][1].y, m_arucoDetector.m_markersCorners[i][2].y, m_arucoDetector.m_markersCorners[i][3].y });
				m_dataPlotter.m_figures["Marker position"].addData({ m_arucoDetector.m_tvecs[markerId][0], m_arucoDetector.m_tvecs[markerId][1], m_arucoDetector.m_tvecs[markerId][2] });
				m_dataPlotter.m_figures["Marker position histogram"].addData({ m_arucoDetector.m_tvecs[markerId][0], m_arucoDetector.m_tvecs[markerId][1], m_arucoDetector.m_tvecs[markerId][2] });
				m_dataPlotter.m_figures["Marker rotation"].addData({ m_arucoDetector.m_rvecs[markerId][0], m_arucoDetector.m_rvecs[markerId][1], m_arucoDetector.m_rvecs[markerId][2] });
				m_dataPlotter.m_figures["Marker rotation histogram"].addData({ m_arucoDetector.m_rvecs[markerId][0], m_arucoDetector.m_rvecs[markerId][1], m_arucoDetector.m_rvecs[markerId][2] });
			}
		#endif // PLOT_ENABLED
	}
	#ifdef PLOT_ENABLED
		m_dataPlotter.m_figures["Table position"].addData({ m_arucoDetector.m_tableTvec[0], m_arucoDetector.m_tableTvec[1], m_arucoDetector.m_tableTvec[2] });
		m_dataPlotter.m_figures["Table rotation"].addData({ m_arucoDetector.m_tableRvec[0], m_arucoDetector.m_tableRvec[1], m_arucoDetector.m_tableRvec[2] });
		m_dataPlotter.m_figures["Own robot position"].addData({ m_arucoDetector.m_ownRobotTvecTableFrame[0], m_arucoDetector.m_ownRobotTvecTableFrame[1], m_arucoDetector.m_ownRobotTvecTableFrame[2] });
		m_dataPlotter.m_figures["Own robot rotation"].addData({ m_arucoDetector.m_ownRobotRvecTableFrame[0], m_arucoDetector.m_ownRobotRvecTableFrame[1], m_arucoDetector.m_ownRobotRvecTableFrame[2] });
	#endif // PLOT_ENABLED
	m_profiler.updateTimepoint(5);
	
	#ifdef CVWINDOW_ENABLED
		m_arucoDetector.drawResults(inputFrame);
	#endif // CVWINDOW_ENABLED
	m_profiler.updateTimepoint(6);
	
	#ifdef CVWINDOW_ENABLED
		cv::imshow("Live", inputFrame);
	#endif // CVWINDOW_ENABLED
	m_profiler.updateTimepoint(7);

	m_profiler.printResults();

	m_iteration++;

	#ifdef CVWINDOW_ENABLED
		int key = cv::waitKey(1);
		if (key == 'q') {
			return 2; //TODO: create RETURN_STOP with define and use it
		} else if (key == 'e') {
			m_arucoDetector.m_nextState = DETECTING_EVERYTHING;
		} else if (key == 't') {
			m_arucoDetector.m_nextState = DETECTING_TABLE;
		} else if (key == 'r') {
			m_arucoDetector.m_nextState = DETECTING_OWN_ROBOT;
		} else if (key == 'w') {
			m_arucoDetector.m_nextState = WAITING;
		} 
	#endif // CVWINDOW_ENABLED
	
	return EXIT_SUCCESS;
}
