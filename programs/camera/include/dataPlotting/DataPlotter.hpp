#pragma once

#ifndef CDR_DATAPROFILER_HPP
#define CDR_DATAPROFILER_HPP

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

#include <spdlog/spdlog.h>

#define NOMINMAX
#include <matplot/matplot.h>

#include "dataPlotting/Figure.hpp"

class DataPlotter {
public:
	std::map<const std::string, Figure> m_figures;

	void addFigure(std::string title, std::vector<std::string> legend, PlottingType plottingType = PLOT);
	void displayFigures();
	void saveDataToCsv();
};

#endif // CDR_DATAPROFILER_HPP