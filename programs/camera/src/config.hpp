#pragma once

#ifndef CDR_CONFIG_HPP
#define CDR_CONFIG_HPP

#include <string>
#include <spdlog/spdlog.h>

// ------------------- Features -------------------
#ifndef NDEBUG
	#define PLOT_ENABLED
	#define CVWINDOW_ENABLED
	//#define COMMUNICATION_ENABLED
	spdlog::level::level_enum logLevel = spdlog::level::debug;
#else
	#define PLOT_ENABLED
	//#define CVWINDOW_ENABLED
	#define COMMUNICATION_ENABLED 
	spdlog::level::level_enum logLevel = spdlog::level::info;
#endif // NDEBUG

std::string serverAddress = "192.168.130.134";
int serverPort =  8910;
std::string cameraSource = "assets/testRessources/Image0001.png"; //"1";
int timeout = 100000;
// For fc = 1.5Hz;
//std::vector<float> filterACoefficients = {-1., 2.25869651, -1.76802919,  0.47250239}, filterBCoefficients = {0.00460379, 0.01381136, 0.01381136, 0.00460379}; // coeffs are reversed, aCoeff[0] is not used
// For fc = 3Hz;
std::vector<float> filterACoefficients = {-1., 1.58188741, -1.01467823, 0.22788136}, filterBCoefficients = {0.02561368, 0.07684105, 0.07684105, 0.02561368}; // coeffs are reversed, aCoeff[0] is not used

#endif //CDR_CONFIG_HPP
