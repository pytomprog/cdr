#pragma once

#ifndef CDR_CONFIG_HPP
#define CDR_CONFIG_HPP

#include <string>
#include <spdlog/spdlog.h>

// ------------------- Features -------------------
#ifndef NDEBUG // Debug mode
	#define PLOT_ENABLED
	#define CVWINDOW_ENABLED
	#define COMMUNICATION_ENABLED
	spdlog::level::level_enum consoleLogLevel = spdlog::level::debug;
#else // Release mode
	//#define PLOT_ENABLED
	//#define CVWINDOW_ENABLED
	#define COMMUNICATION_ENABLED
	spdlog::level::level_enum consoleLogLevel = spdlog::level::info;
#endif // NDEBUG


const std::string cameraSource = "assets/testRessources/Image0001.png"; //"0";

const int timeout = 100000;

// For fc = 1.5Hz;
//const std::vector<float> filterACoefficients = {-1., 2.25869651, -1.76802919,  0.47250239}, filterBCoefficients = {0.00460379, 0.01381136, 0.01381136, 0.00460379}; // coeffs are reversed, aCoeff[0] is not used
// For fc = 3Hz;
const std::vector<float> filterACoefficients = {-1., 1.58188741, -1.01467823, 0.22788136}, filterBCoefficients = {0.02561368, 0.07684105, 0.07684105, 0.02561368}; // coeffs are reversed, aCoeff[0] is not used

const std::string mqttServerURI = "mqtt://10.49.113.10:1883";
const std::string mqttClientId = "static_camera";
const std::string mqttPersistDir = "persist";
const std::string mqttOwnRobotPositionTopic = "/static_camera/own_robot_position";
const int mqttOwnRobotPositionQOS = 0;
const std::string mqttAdvRobotPositionTopic = "/static_camera/adv_robot_position";
const int mqttAdvRobotPositionQOS = 1;

#endif //CDR_CONFIG_HPP
