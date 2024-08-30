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

std::string cameraSource = "1";// "VID20240822151428.mp4";