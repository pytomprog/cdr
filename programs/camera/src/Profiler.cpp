#include "Profiler.hpp"

void Profiler::updateTimepoint(size_t timepointIndex) {
	if (timepointIndex < m_timepoints.size()) {
		m_timepoints[timepointIndex] = cv::getTickCount();
	} else {
		int nbToAdd = timepointIndex - m_timepoints.size() + 1;
		for (int i = 0; i < nbToAdd; i++) {
			m_timepoints.push_back(cv::getTickCount());
		}
	}
}

void Profiler::printResults() {
	for (int i = 0; i <= m_timepoints.size() - 2; i++)
		spdlog::debug("Between t{} and t{}: {}sec", i, i + 1, (m_timepoints[i + 1] - m_timepoints[i]) / cv::getTickFrequency());
	spdlog::debug("Total time: {}sec\n", (m_timepoints[m_timepoints.size() - 1] - m_timepoints[0]) / cv::getTickFrequency());
}
