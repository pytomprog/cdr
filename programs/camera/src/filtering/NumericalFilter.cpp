#include "filtering/NumericalFilter.hpp"

NumericalFilter::NumericalFilter(std::vector<float> aCoefficients, std::vector<float> bCoefficients, float initialValue) {
	m_aCoefficients = aCoefficients;
	m_bCoefficients = bCoefficients;
	
	m_previousFilteredValues = m_previousRawValues = std::vector<float>(m_bCoefficients.size(), initialValue);
}

float NumericalFilter::filter(float newRawValue) {
	/*
	# Filter the signal
	Nb = len(b)
	# ylift = np.array(y) allows to have the firt values good and not at 0
	yfilt = np.zeros(len(y)) #np.array(y) #np.zeros(len(y))
	for m in range(3,len(y)):
		yfilt[m] = b[0]*y[m]
		for i in range(1,Nb):
			yfilt[m] += a[i]*yfilt[m-i] + b[i]*y[m-i]
	*/
	
	float newFilteredValue = m_bCoefficients[0]*newRawValue;
	//for i in range(1,Nb):
	//	yfilt[m] += a[i]*yfilt[m-i] + b[i]*y[m-i]
	for (int i = 1; i < m_bCoefficients.size(); i++) {
		newFilteredValue += m_aCoefficients[i]*m_previousFilteredValues[m_previousFilteredValues.size() - i] + m_bCoefficients[i]*m_previousRawValues[m_previousRawValues.size() - i];
	}
	
	m_previousRawValues.erase(m_previousRawValues.begin());
	m_previousRawValues.push_back(newRawValue);
	
	m_previousFilteredValues.erase(m_previousFilteredValues.begin());
	m_previousFilteredValues.push_back(newFilteredValue);
	
	return newFilteredValue;
}
