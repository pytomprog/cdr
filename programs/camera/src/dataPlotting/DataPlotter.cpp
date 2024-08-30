#include "dataPlotting/DataPlotter.hpp"

void DataPlotter::addFigure(std::string title, std::vector<std::string> legend, PlottingType plottingType) {
	m_figures[title] = Figure(title, legend, plottingType);
}

void DataPlotter::displayFigures() {
	for (std::pair<const std::string, Figure> figurePair : m_figures)
		figurePair.second.display();
	matplot::show();
}
