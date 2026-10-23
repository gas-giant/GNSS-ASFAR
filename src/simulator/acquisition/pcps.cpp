#include "simulator/acquisition/fft.h"
#include "simulator/signals/gps_l1ca.h"
#include "simulator/signals/signal.h"
#include "simulator/complex.h"
#include "common.h"
#include <vector>
#include <iostream>
#include <string>

#define DEBUG_PRINT_ON 0

static void DebugPrintComplexDouble(const Complex<double>& x, const std::string& xName = "x") {
    if (DEBUG_PRINT_ON) {
        std::cout << "[DEBUG]: " << xName << " = [" 
            << x.real << ", "
            << x.imag << "]" << std::endl;
    }
}

static void DebugPrintDouble(const double& x, const std::string& xName = "x") {
    if (DEBUG_PRINT_ON) {
        std::cout << "[DEBUG]: " << xName << " = "
            << x << std::endl;
    }
}

// Parallel Code Phase Searching
S32 pcps(
    const std::vector<Complex<double>>& ifData,
    const SampleParameter& sampleParam,
    const Signal& signal,
    const S32 prn,
    const S32 circleIntegralNum,
    const double doppler,
    std::vector<double>& energySequence
) {
    // Doppler phase compensation
    double angle = -2.0 * M_PI * doppler / sampleParam.fs;
    Complex<double> phaseCompensationStep = Complex<double>(cos(angle), sin(angle));
    Complex<double> phaseCompensationFactor = Complex<double>(1, 0);
    std::vector<Complex<double>> compensatedData(ifData.size());
    for (int i = 0; i < ifData.size(); ++i) {
        compensatedData[i] = ifData[i] * phaseCompensationFactor;
        phaseCompensationFactor = phaseCompensationFactor * phaseCompensationStep;
    }
    
    // Circle integral
    std::vector<Complex<double>> circleIntegral(sampleParam.length, {0, 0});
    for (int i = 0; i < circleIntegralNum; ++i) {
        for (int j = 0; j < sampleParam.length; ++j) {
            circleIntegral[j] = 
                circleIntegral[j] +
                compensatedData[i * sampleParam.length + j];
        }
    }
    DebugPrintComplexDouble(circleIntegral[0], "circleIntegral[0]");

    // Signal spectrum
    std::vector<Complex<double>>& signalSpectrum = circleIntegral;
    S32 ret = fft(&signalSpectrum[0], sampleParam.length);
    if (ret != FFT_SUCCESS) {
        std::cout << "FFT error: " << ret << std::endl;
        return 1;
    }

    // Local PRN code
    std::vector<S32> sampledLocalCode = signal.generateSampledCode(sampleParam, prn);

    // Local spectrum
    std::vector<Complex<double>> localSpectrum(sampleParam.length, {0, 0});
    for (int i = 0; i < sampleParam.length; ++i) {
        localSpectrum[i].real = sampledLocalCode[i];
    }
    ret = fft(&localSpectrum[0], sampleParam.length);
    if (ret != FFT_SUCCESS) {
        std::cout << "FFT error: " << ret << std::endl;
        return 1;
    }

    // Correlation spectrum
    for (int i = 0; i < sampleParam.length; ++i) {
        signalSpectrum[i] = signalSpectrum[i] * localSpectrum[i].conjugate();
    }
    std::vector<Complex<double>>& correlationSpectrum = signalSpectrum;
    DebugPrintComplexDouble(correlationSpectrum[0], "correlationSpectrum[0]");

    // Correlation value
    ret = ifft(&correlationSpectrum[0], sampleParam.length);
    if (ret != FFT_SUCCESS) {
        std::cout << "FFT error: " << ret << std::endl;
        return 1;
    }
    std::vector<Complex<double>>& correlationSequence = correlationSpectrum;
    DebugPrintComplexDouble(correlationSequence[0], "correlationSequence[0]");

    // Energy
    for (int i = 0; i < sampleParam.length; ++i) {
        energySequence[i] = 
            correlationSequence[i].real * correlationSequence[i].real +
            correlationSequence[i].imag * correlationSequence[i].imag;
    }
    return 0;
}