#pragma once

#ifndef CDR_PROFILER_HPP
#define CDR_PROFILER_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <opencv2/core.hpp>

#include <spdlog/spdlog.h>

class Profiler {
public:
	std::vector<int64> m_timepoints;

	void updateTimepoint(size_t timepointIndex);
	void printResults();
};


#endif // CDR_PROFILER_HPP