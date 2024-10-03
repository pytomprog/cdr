#pragma once

#ifndef CDR_NUMERICALFILTER_HPP
#define CDR_NUMERICALFILTER_HPP

#include <vector>

class NumericalFilter {
public:
	std::vector<float> m_aCoefficients, m_bCoefficients;

	std::vector<float> m_previousRawValues, m_previousFilteredValues;

	NumericalFilter(std::vector<float> aCoefficients, std::vector<float> bCoefficients, float initialValue);

	float filter(float newRawValue);
};

#endif // CDR_NUMERICALFILTER_HPP
