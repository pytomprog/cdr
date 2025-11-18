#include "Main.hpp"

std::string getTimestampedFilename() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << "/logs/cameraProgram/log_" << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".log";
    return oss.str();
}

uint8_t COLOR_CHOICE_PIN = 16;
int main(int argc, char* argv[]) {
	/*if (argc != 2) {
		std::cerr << "Please provide 1 argument for the team: y or b" << std::endl;
		return 1;
	}
	Main m(argv[1][0] == 'y');*/
	
	if (!bcm2835_init())
		return 1;
	
	// Set RPI pin P1-15 to be an input
	bcm2835_gpio_fsel(COLOR_CHOICE_PIN, BCM2835_GPIO_FSEL_INPT);
	// with a pullup
	bcm2835_gpio_set_pud(COLOR_CHOICE_PIN, BCM2835_GPIO_PUD_UP); 
	// Now get the value
	bool isYellow = bcm2835_gpio_lev(COLOR_CHOICE_PIN);
	
	Main m(isYellow);
	
	return m.run();
}

Main::Main(bool isYellow): m_arucoDetector(m_camera, m_profiler, (isYellow) ? YELLOW_TEAM : BLUE_TEAM) {}

int Main::run() {
	using namespace std::chrono_literals;
	
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(consoleLogLevel);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(getTimestampedFilename(), true);
    file_sink->set_level(spdlog::level::debug);    // Log debug and above to file

    // Combine sinks into one logger
    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
	logger->flush_on(spdlog::level::debug); //file_sink->flush_every(std::chrono::milliseconds(500));
	spdlog::set_default_logger(logger);
	spdlog::set_level(spdlog::level::debug);
	
	spdlog::info("Team selection: {}", (m_arucoDetector.m_ownTeam == YELLOW_TEAM) ? "Yellow" : "Blue");

	spdlog::debug("OpenCV infos: {}", cv::getBuildInformation().c_str());

	#ifdef PLOT_ENABLED
		//m_dataPlotter.addFigure("Marker position", { "tx", "ty", "tz" }, PLOT);
		//m_dataPlotter.addFigure("Marker rotation", { "rx", "ry", "rz" }, PLOT);
		//m_dataPlotter.addFigure("Marker position histogram", { "tx", "ty", "tz" }, HIST);
		//m_dataPlotter.addFigure("Marker rotation histogram", { "rx", "ry", "rz" }, HIST);
		//m_dataPlotter.addFigure("Corners x position", { "0", "1", "2", "3" }, PLOT);
		//m_dataPlotter.addFigure("Corners y position", { "0", "1", "2", "3" }, PLOT);
		m_dataPlotter.addFigure("Table position", { "tableTx", "tableTy", "tableTz" }, PLOT);
		m_dataPlotter.addFigure("Table rotation", { "tableRx", "tableRy", "tableRz" }, PLOT);
		m_dataPlotter.addFigure("Own robot position", { "robotTx", "robotTy", "robotTz", "robotTxFiltered", "robotTyFiltered" }, PLOT);
		m_dataPlotter.addFigure("Own robot rotation", { "robotRx", "robotRy", "robotRz", "robotRzFiltered" }, PLOT);
		//m_dataPlotter.addFigure("Own robot position histogram", { "robotTxHist", "robotTyHist", "robotTzHist" }, HIST);
		//m_dataPlotter.addFigure("Own robot rotation histogram", { "robotRxHist", "robotRyHist", "robotRzHist" }, HIST);
	#endif // PLOT_ENABLED
	
	#ifdef COMMUNICATION_ENABLED
		m_serverAddress.sin_family = AF_INET;
		m_serverAddress.sin_port = htons(serverPort);
		m_serverAddress.sin_addr.s_addr = inet_addr(serverAddress.c_str());
	#endif // COMMUNICATION_ENABLED
	
	processFrameFunction = [this](cv::Mat* inputFramePtr) {return this->step(inputFramePtr);};
	m_startingTick = cv::getTickCount();
	
	m_camera.open(cameraSource);
	spdlog::info("Camera opened !");
		
	do {
		#ifdef COMMUNICATION_ENABLED
			m_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
			m_failedMessagesCount = 0;
			int connectionReturnCode;
			bool connected = false;
			while (!connected) {
				spdlog::error("Trying to connect to the server, on {}:{} ...", serverAddress, serverPort);
				connectionReturnCode = connect(m_clientSocket, (struct sockaddr*)&m_serverAddress, sizeof(m_serverAddress));
				spdlog::info("Return code: {}", connectionReturnCode);
				if (connectionReturnCode >= 0) {
					connected = true;
				} else {
					spdlog::error("Failed to connect to the server, on {}:{}, errno: {}, Retrying ...", serverAddress, serverPort, errno);
					std::this_thread::sleep_for(500ms);
				}
			}
			fcntl(m_clientSocket, F_SETFL, O_NONBLOCK);
			spdlog::info("Connected to the server, on {}:{}", serverAddress, serverPort);
		#endif // COMMUNICATION_ENABLED
		
		spdlog::info("Starting computing robots position...");	
		m_camera.run();
		
		#ifdef COMMUNICATION_ENABLED
			spdlog::info("Closing code: {}", close(m_clientSocket));
		#endif // COMMUNICATION_ENABLED
	} while (stopCode != 2);
	
	m_camera.close();
	spdlog::info("Camera closed");

	float totalTime = (cv::getTickCount() - m_startingTick) / cv::getTickFrequency();
	spdlog::info("Total iterations {} over {} seconds at a mean rate of {} Hz", m_iteration, totalTime, m_iteration / totalTime);

	#ifdef PLOT_ENABLED
		m_dataPlotter.saveDataToCsv();
		m_dataPlotter.displayFigures();
	#endif // PLOT_ENABLED

	return EXIT_SUCCESS;
}

int Main::step(cv::Mat* inputFramePtr) {
	cv::Mat inputFrame = *inputFramePtr;
	if (inputFrame.empty()) {
		spdlog::error("Processing empty frame !");
		stopCode = EXIT_FAILURE;
		return EXIT_FAILURE;
	}
	
	#ifndef CVWINDOW_ENABLED
		if ((cv::getTickCount() - m_startingTick) / cv::getTickFrequency() > timeout) {
			stopCode = 2; //TODO: create EXIT_STOP with define and use it
			return 2;
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
				//m_dataPlotter.m_figures["Marker position"].addData({ m_arucoDetector.m_tvecs[markerId][0], m_arucoDetector.m_tvecs[markerId][1], m_arucoDetector.m_tvecs[markerId][2] });
				//m_dataPlotter.m_figures["Marker position histogram"].addData({ m_arucoDetector.m_tvecs[markerId][0], m_arucoDetector.m_tvecs[markerId][1], m_arucoDetector.m_tvecs[markerId][2] });
				//m_dataPlotter.m_figures["Marker rotation"].addData({ m_arucoDetector.m_rvecs[markerId][0], m_arucoDetector.m_rvecs[markerId][1], m_arucoDetector.m_rvecs[markerId][2] });
				//m_dataPlotter.m_figures["Marker rotation histogram"].addData({ m_arucoDetector.m_rvecs[markerId][0], m_arucoDetector.m_rvecs[markerId][1], m_arucoDetector.m_rvecs[markerId][2] });
			}
		#endif // PLOT_ENABLED
	}
	
	float ownRobotTxFiltered = m_ownRobotTxFilter.filter(m_arucoDetector.m_ownRobotTvecTableFrame[0]);
	float ownRobotTyFiltered = m_ownRobotTyFilter.filter(m_arucoDetector.m_ownRobotTvecTableFrame[1]);
	float ownRobotRzCosFiltered = m_ownRobotRxCosFilter.filter(cos(m_arucoDetector.m_ownRobotRvecTableFrame[2]));
	float ownRobotRzSinFiltered = m_ownRobotRxSinFilter.filter(sin(m_arucoDetector.m_ownRobotRvecTableFrame[2]));
	float ownRobotRzFiltered = acos(ownRobotRzCosFiltered);
	if (ownRobotRzSinFiltered < 0.f)
		ownRobotRzFiltered = -ownRobotRzFiltered;
		
	float advRobotTxFiltered = m_advRobotTxFilter.filter(m_arucoDetector.m_advRobotTvecTableFrame[0]);
	float advRobotTyFiltered = m_advRobotTyFilter.filter(m_arucoDetector.m_advRobotTvecTableFrame[1]);
	float advRobotRzCosFiltered = m_advRobotRxCosFilter.filter(cos(m_arucoDetector.m_advRobotRvecTableFrame[2]));
	float advRobotRzSinFiltered = m_advRobotRxSinFilter.filter(sin(m_arucoDetector.m_advRobotRvecTableFrame[2]));
	float advRobotRzFiltered = acos(advRobotRzCosFiltered);
	if (advRobotRzSinFiltered < 0.f)
		advRobotRzFiltered = -advRobotRzFiltered;
	
	#ifdef PLOT_ENABLED
		m_dataPlotter.m_figures["Table position"].addData({ m_arucoDetector.m_tableTvec[0], m_arucoDetector.m_tableTvec[1], m_arucoDetector.m_tableTvec[2] });
		m_dataPlotter.m_figures["Table rotation"].addData({ m_arucoDetector.m_tableRvec[0], m_arucoDetector.m_tableRvec[1], m_arucoDetector.m_tableRvec[2] });
		m_dataPlotter.m_figures["Own robot position"].addData({ m_arucoDetector.m_ownRobotTvecTableFrame[0], m_arucoDetector.m_ownRobotTvecTableFrame[1], m_arucoDetector.m_ownRobotTvecTableFrame[2], robotTxFiltered, robotTyFiltered });
		m_dataPlotter.m_figures["Own robot rotation"].addData({ m_arucoDetector.m_ownRobotRvecTableFrame[0], m_arucoDetector.m_ownRobotRvecTableFrame[1], m_arucoDetector.m_ownRobotRvecTableFrame[2], robotRzFiltered });
		//m_dataPlotter.m_figures["Own robot position histogram"].addData({ m_arucoDetector.m_ownRobotTvecTableFrame[0], m_arucoDetector.m_ownRobotTvecTableFrame[1], m_arucoDetector.m_ownRobotTvecTableFrame[2] });
		//m_dataPlotter.m_figures["Own robot rotation histogram"].addData({ m_arucoDetector.m_ownRobotRvecTableFrame[0], m_arucoDetector.m_ownRobotRvecTableFrame[1], m_arucoDetector.m_ownRobotRvecTableFrame[2] });
	
	#endif // PLOT_ENABLED
	m_profiler.updateTimepoint(5);
	
	#ifdef COMMUNICATION_ENABLED
		std::string message;
		int returnedCode;
		message = fmt::format("A {} {} {}\n", ownRobotTxFiltered, ownRobotTyFiltered, ownRobotRzFiltered + 1.575f).c_str(); //TODO: remove 1.575f and make it cleaner
		//spdlog::info("Sending \"{}\" to the server", message);
		returnedCode = send(m_clientSocket, message.c_str(), message.size(), 0);
		if (returnedCode < 0) { // failed to send message
			m_failedMessagesCount++;
			if (m_failedMessagesCount >= 3) {
				spdlog::error("Failed 3 times to send messages");
				stopCode = EXIT_FAILURE;
				return EXIT_FAILURE;
			}
		}
		
		message = fmt::format("B {} {} {}", advRobotTxFiltered, advRobotTyFiltered, advRobotRzFiltered + 1.575f).c_str(); //TODO: remove 1.575f and make it cleaner
		//spdlog::info("Sending \"{}\" to the server", message);
		returnedCode = send(m_clientSocket, message.c_str(), message.size(), 0);
		if (returnedCode < 0) { // failed to send message
			m_failedMessagesCount++;
			if (m_failedMessagesCount >= 3) {
				spdlog::error("Failed 3 times to send messages");
				stopCode = EXIT_FAILURE;
				return EXIT_FAILURE;
			}
		}	
	#endif // COMMUNICATION_ENABLED
	
	#ifdef CVWINDOW_ENABLED
		//m_arucoDetector.drawResults(inputFrame);
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
		} else if (key == 'b') {
			m_arucoDetector.m_nextState = DETECTING_BOTH_ROBOTS;
		} else if (key == 'w') {
			m_arucoDetector.m_nextState = WAITING;
		} 
	#endif // CVWINDOW_ENABLED
	
	stopCode = EXIT_SUCCESS;
	return EXIT_SUCCESS;
}
