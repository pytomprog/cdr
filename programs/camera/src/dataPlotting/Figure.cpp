#include "dataPlotting/Figure.hpp"

Figure::Figure() {}

Figure::Figure(std::string title, std::vector<std::string> legend, PlottingType plottingType) {
	m_title = title;
	m_legend = legend;
	m_plottingType = plottingType;
	for (int i = 0; i < m_legend.size(); i++) m_dataTimelines.push_back(std::vector<float>());
}

void Figure::addData(std::vector<float> data) {
	for (size_t i = 0; i < data.size(); i++) {
		m_dataTimelines[i].push_back(data[i]);
	}
}

void Figure::display() {
	matplot::figure();
	//matplot::subplot(totalLines, totalRows, index);
	matplot::title(m_title);
	matplot::legend(m_legend);
	matplot::hold(matplot::on);
	for (std::vector<float> m_dataTimeline : m_dataTimelines) {
		switch (m_plottingType){
		//case SCATTER:
		//	matplot::scatter(m_dataTimeline);
		case HIST:
			matplot::hist(m_dataTimeline);
			break;
		default:
			matplot::plot(m_dataTimeline);
			break;
		}
	}
}