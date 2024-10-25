#pragma once

#ifndef CDR_MAIN_HPP
#define CDR_MAIN_HPP

#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <cmath>
#include <functional>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <unistd.h> 

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "config.hpp"
#include "aruco/ArucoDetector.hpp"
#include "camera/Camera.hpp"
#include "Profiler.hpp"
#ifdef PLOT_ENABLED
	#include "dataPlotting/DataPlotter.hpp"
	#include "dataPlotting/Figure.hpp"
#endif // PLOT_ENABLED
#include "filtering/NumericalFilter.hpp"
#include "SpdlogUtils.hpp"


std::function<int(cv::Mat*)> processFrameFunction;
class Main {
public:
	Profiler m_profiler;
	#ifdef PLOT_ENABLED
		DataPlotter m_dataPlotter;
	#endif // PLOT_ENABLED
	Camera m_camera = Camera(2304, 1296);
	ArucoDetector m_arucoDetector = ArucoDetector(m_camera, m_profiler, BLUE_TEAM, 3);
	
	int stopCode = 0;
	
	NumericalFilter m_robotTxFilter = NumericalFilter(filterACoefficients, filterBCoefficients, 0.f);
	NumericalFilter m_robotTyFilter = NumericalFilter(filterACoefficients, filterBCoefficients, 0.f);
	NumericalFilter m_robotRxCosFilter = NumericalFilter(filterACoefficients, filterBCoefficients, cos(0.f));
	NumericalFilter m_robotRxSinFilter = NumericalFilter(filterACoefficients, filterBCoefficients, sin(0.f));
	//NumericalFilter m_robotRzFilter(filterACoefficients, filterBCoefficients, 0.f);
	
	int m_iteration = 0;
	int64 m_startingTick;
	
	#ifdef COMMUNICATION_ENABLED
		int m_clientSocket;
		sockaddr_in m_serverAddress;
		bool m_connectedToServer;
		int m_failedMessagesCount;
	#endif // COMMUNICATION_ENABLED
	
	
	Main();

	int run();
	int step(cv::Mat* inputFramePtr);
};

int main();

#endif // CDR_MAIN_HPP
