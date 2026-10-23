#ifndef SIGNAL_H
#define SIGNAL_H

#include "common.h"
#include <vector>

class SampleParameter {
public:
    double  fs;                 // frequency of sampling (Hz)
    U32     length;             // length of sampled sequence
    double  fdCode;             // Doppler frequency of code (Hz)
    double  initialCodePhase;   // initial code phase (chip)
    SampleParameter(
        double fs, 
        U32 length,
        double fdCode = 0,
        double initialCodePhase = 0
    ) : 
    fs(fs),
    length(length),
    fdCode(fdCode),
    initialCodePhase(initialCodePhase) {
        if ((fdCode < 0) ||
            (fs < 0) || 
            (F_EZ(fs)) ||
            (initialCodePhase < 0) ||
            (initialCodePhase > 1023) || 
            (F_EQ(initialCodePhase, 1023)) || 
            (length == 0)
        ) {
            throw std::invalid_argument("sampleParam");
        }
    }
};

class Signal {
protected:
    double fCode_;       // frequency of PRN code (Hz)
    double codeLength_;  // length of PRN code chip
public:
    double getFrequencyOfCode(void) const {
        return fCode_;
    }
    double getCodeLength(void) const {
        return codeLength_;
    }
    virtual std::vector<S32> generateCodeChips(S32 prn) const = 0;
    virtual std::vector<S32> generateSampledCode
        (const SampleParameter& sampleParam, S32 prn) const = 0;
};

#endif // SIGNAL_H
