#ifndef PCPS_H
#define PCPS_H

#include "simulator/complex.h"
#include "simulator/signals/signal.h"
#include "common.h"
#include <vector>

// Parallel Code Phase Searching
// double
S32 pcps(
    const std::vector<Complex<double>>& ifData,
    const SampleParameter& sampleParam,
    const Signal& signal,
    const S32 prn,
    const S32 circleIntegralNum,
    const double doppler,
    std::vector<double>& energySequence
);

// TODO: fixed point

#endif // PCPS_H