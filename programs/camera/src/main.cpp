#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <map>
#include <cmath>

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "config.hpp"
#include "Camera.hpp"
#include "aruco/ArucoDetector.hpp"
#include "Profiler.hpp"
#ifdef PLOT_ENABLED
	#include "dataPlotting/DataPlotter.hpp"
	#include "dataPlotting/Figure.hpp"
#endif // PLOT_ENABLED
#include "SpdlogUtils.hpp"

#ifdef ARM
int test = 0;
#endif //ARM

int main() {
	spdlog::set_level(logLevel);

	spdlog::info("OpenCV infos: {}", cv::getBuildInformation().c_str());

	Profiler profiler;

	#ifdef PLOT_ENABLED
		DataPlotter dataPlotter;
		dataPlotter.addFigure("Marker position", { "tx", "ty", "tz" }, PLOT);
		dataPlotter.addFigure("Marker rotation", { "rx", "ry", "rz" }, PLOT);
		dataPlotter.addFigure("Marker position histogram", { "tx", "ty", "tz" }, HIST);
		dataPlotter.addFigure("Marker rotation histogram", { "rx", "ry", "rz" }, HIST);
		dataPlotter.addFigure("Corners x position", { "0", "1", "2", "3" }, PLOT);
		dataPlotter.addFigure("Corners y position", { "0", "1", "2", "3" }, PLOT);
	#endif // PLOT_ENABLED

	Camera camera(1920, 1080);
	camera.open(cameraSource);
	spdlog::info("Camera intrinsic matrix:\n{}", camera.m_intrinsicMatrix);
	spdlog::info("Camera distortion coefficients:\n{}", camera.m_distortionCoefficients);

	ArucoDetector arucoDetector(camera, profiler, 22);

	cv::Mat inputFrame;
	
	int iteration = 0;
	int64 startingTick(cv::getTickCount());
	while ((cv::getTickCount() - startingTick) / cv::getTickFrequency() < 20) {
		spdlog::debug("Iteration {}:", iteration);

		profiler.updateTimepoint(0);

		camera.getImage(inputFrame);
		if (inputFrame.empty())
			break;
		profiler.updateTimepoint(1);

		arucoDetector.getMarkers3dPosition(inputFrame);
		profiler.updateTimepoint(5);

		spdlog::debug("inputFrame: cols: {}, rows: {}, channels: {}", inputFrame.cols, inputFrame.rows, inputFrame.channels());
		spdlog::debug("croppedFrame: cols: {}, rows: {}, channels: {}", arucoDetector.m_croppedFrame.cols, arucoDetector.m_croppedFrame.rows, arucoDetector.m_croppedFrame.channels());
		spdlog::debug("Makers detected: {}:", arucoDetector.m_markersId.size());
		for (int i = 0; i < arucoDetector.m_markersId.size(); i++) {
			int markerId = arucoDetector.m_markersId[i];
			spdlog::debug("\t- {}:", markerId);
			spdlog::debug("\t\tPosition: {}", arucoDetector.m_tvecs[markerId]);
			spdlog::debug("\t\tRotation: {}", arucoDetector.m_rvecs[markerId]);
			#ifdef PLOT_ENABLED
				if (markerId == arucoDetector.m_focusMarkerId) {
					dataPlotter.m_figures["Corners x position"].addData({ arucoDetector.m_markersCorners[i][0].x, arucoDetector.m_markersCorners[i][1].x, arucoDetector.m_markersCorners[i][2].x, arucoDetector.m_markersCorners[i][3].x });
					dataPlotter.m_figures["Corners y position"].addData({ arucoDetector.m_markersCorners[i][0].y, arucoDetector.m_markersCorners[i][1].y, arucoDetector.m_markersCorners[i][2].y, arucoDetector.m_markersCorners[i][3].y });
					dataPlotter.m_figures["Marker position"].addData({ arucoDetector.m_tvecs[markerId][0], arucoDetector.m_tvecs[markerId][1], arucoDetector.m_tvecs[markerId][2] });
					dataPlotter.m_figures["Marker position histogram"].addData({ arucoDetector.m_tvecs[markerId][0], arucoDetector.m_tvecs[markerId][1], arucoDetector.m_tvecs[markerId][2] });
					dataPlotter.m_figures["Marker rotation"].addData({ arucoDetector.m_rvecs[markerId][0], arucoDetector.m_rvecs[markerId][1], arucoDetector.m_rvecs[markerId][2] });
					dataPlotter.m_figures["Marker rotation histogram"].addData({ arucoDetector.m_rvecs[markerId][0], arucoDetector.m_rvecs[markerId][1], arucoDetector.m_rvecs[markerId][2] });
				}
			#endif // PLOT_ENABLED
		}
		profiler.updateTimepoint(6);
		
		#ifdef CVWINDOW_ENABLED
			arucoDetector.drawMarkers(inputFrame);
		#endif // CVWINDOW_ENABLED
		profiler.updateTimepoint(7);

		#ifdef CVWINDOW_ENABLED
			cv::imshow("Live", inputFrame);
		#endif // CVWINDOW_ENABLED
		profiler.updateTimepoint(8);

		profiler.printResults();

		iteration++;

		#ifdef CVWINDOW_ENABLED
			if (cv::waitKey(1) == 'q')
				break;
		#endif // CVWINDOW_ENABLED
	}
	camera.close();
	#ifdef CVWINDOW_ENABLED
		cv::destroyWindow("Live");
	#endif // CVWINDOW_ENABLED

	float totalTime = (cv::getTickCount() - startingTick) / cv::getTickFrequency();
	spdlog::info("Total iterations {} over {} seconds at a mean rate of {} Hz", iteration, totalTime, iteration / totalTime);

	#ifdef PLOT_ENABLED
		dataPlotter.displayFigures();
	#endif // PLOT_ENABLED

	return EXIT_SUCCESS;
}