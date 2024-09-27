#include "dataPlotting/DataPlotter.hpp"

void DataPlotter::addFigure(std::string title, std::vector<std::string> legend, PlottingType plottingType) {
	m_figures[title] = Figure(title, legend, plottingType);
}

void DataPlotter::displayFigures() {
	for (std::pair<const std::string, Figure> figurePair : m_figures)
		figurePair.second.display();
	matplot::show();
}

void DataPlotter::saveDataToCsv() {
	std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    char timeText[20];
	std::strftime(timeText, sizeof(timeText), "%Y-%m-%d_%H:%M:%S", localTime);
	
	std::ofstream csvFile(fmt::format("./data/data_{}.csv", timeText));
	int dataSize = m_figures.begin()->second.m_dataTimelines[0].size();
	for (int t=-1; t<dataSize; t++) {
		for (std::map<const std::string, Figure>::iterator iter = m_figures.begin(); iter != m_figures.end(); iter++) {
			Figure& figure = iter->second;
			for (size_t i=0; i<figure.m_legend.size(); i++) {
				if (t == -1) {
					csvFile << figure.m_legend[i];
				} else {
					csvFile << figure.m_dataTimelines[i][t];
				}
				if (!(iter == std::prev(m_figures.end()) && i == figure.m_legend.size()-1))
					csvFile << ";";
			}
		}
		if (t < dataSize-1)
			csvFile << std::endl;
	}
	csvFile.close();
}
