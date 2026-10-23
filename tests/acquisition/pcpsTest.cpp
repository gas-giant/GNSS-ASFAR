#include "simulator/acquisition/pcps.h"
#include "simulator/complex.h"
#include "simulator/signals/gps_l1ca.h"
#include "acquisition/data/ifData_gpsl1ca.h"
#include "dumpVector.h"
#include <gtest/gtest.h>
#include <vector>

#define DEBUG_PRINT_ON 1

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

TEST(pcpsTest, GPSL1CA_double) {
    std::vector<Complex<double>> gpsL1caIf(GPS_L1CA_IF_LEN);
    // IF complex data
    for (int i = 0; i < GPS_L1CA_IF_LEN; ++i) {
        gpsL1caIf[i].real = g_gpsL1caIfI[i];
        gpsL1caIf[i].imag = g_gpsL1caIfQ[i];
    }
    
    // fs = 16.384MHz
    double fs = 16384000;
    // samples length: 1ms
    U32 length = 16384;
    SampleParameter sampleParam(fs, length);
    // GNSS system and prn
    SignalGpsL1CA signal;
    S32 prn = 1;
    // number of circle integral
    S32 circleIntegralNum = 5;
    // energy sequence
    std::vector<double> energySequence(sampleParam.length);
    // doppler (Hz)
    // [-2500, +2500]
    const double dopplerResolution = 50;
    const S32 dopplerRangeSize = 101;
    std::vector<double> dopplerRange(dopplerRangeSize);
    for (int i = 0; i < dopplerRangeSize; ++i) {
        dopplerRange[i] = -(dopplerRangeSize / 2) * dopplerResolution + i * dopplerResolution;
    }

    // searching
    std::vector<std::vector<double>> energyMatrix(dopplerRangeSize, 
        std::vector<double> (sampleParam.length, 0));
    for (int i = 0; i < dopplerRangeSize; ++i) {
        int ret = pcps(gpsL1caIf, sampleParam, signal, prn, 
            circleIntegralNum, dopplerRange[i], energyMatrix[i]);
        ASSERT_EQ(ret, 0);
    }
    DumpMatrix(energyMatrix, dopplerRange, "pcps_output/energy_matrix.bin");
    

    // Peak
    double maxValue = 0;
    S32 dopplerIndex = -1;
    S32 phaseIndex = -1;
    for (int i = 0; i < dopplerRangeSize; ++i) {
        for (int j = 0; j < sampleParam.length; ++j) {
            if (energyMatrix[i][j] > maxValue) {
                maxValue = energyMatrix[i][j];
                dopplerIndex = i;
                phaseIndex = j;
            }
        }
    }
    double peakDoppler = -(dopplerRangeSize / 2) * dopplerResolution + 
    dopplerIndex * dopplerResolution;
    double delay = phaseIndex * signal.getCodeLength() / sampleParam.length;
    double peakPhase = std::fmod(signal.getCodeLength() - delay, signal.getCodeLength());
    DebugPrintDouble(maxValue, "peak");
    DebugPrintDouble(peakDoppler, "peakDoppler");
    DebugPrintDouble(peakPhase, "peakPhase");

    EXPECT_NEAR(peakDoppler, 500.0, dopplerResolution / 2);
    EXPECT_NEAR(peakPhase, 500.0, 0.5);
}