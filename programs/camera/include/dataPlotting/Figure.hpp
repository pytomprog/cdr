#pragma once

#ifndef CDR_FIGURE_HPP
#define CDR_FIGURE_HPP

#include <string>
#include <vector>

#define NOMINMAX
#include <matplot/matplot.h>

enum PlottingType {
	PLOT,
	SCATTER,
	HIST
};

class Figure {
public:
	std::string m_title;
	std::vector<std::string> m_legend;
	std::vector<std::vector<float>> m_dataTimelines;
	PlottingType m_plottingType;

	Figure();
	Figure(std::string title, std::vector<std::string> legend, PlottingType plottingType = PLOT);

	void addData(std::vector<float> data);
	void display();

};

#endif // CDR_FIGURE_HPP