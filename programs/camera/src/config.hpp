#pragma once

#ifndef CDR_CONFIG_HPP
#define CDR_CONFIG_HPP

#include <string>
#include <spdlog/spdlog.h>

// ------------------- Features -------------------
#ifndef NDEBUG
	#define PLOT_ENABLED
	#define CVWINDOW_ENABLED
	spdlog::level::level_enum logLevel = spdlog::level::debug;
#else
	#define PLOT_ENABLED
	#define CVWINDOW_ENABLED
	spdlog::level::level_enum logLevel = spdlog::level::info;
#endif // NDEBUG

std::string cameraSource = "assets/testRessources/Image0001.png"; //"1";

#endif //CDR_CONFIG_HPP
