#ifndef GPS_L1CA_H
#define GPS_L1CA_H

#include "simulator/signals/signal.h"

class SignalGpsL1CA : public Signal {
public:
    SignalGpsL1CA() {
        codeLength_ = 1023;
        fCode_ = 1023000;
    }
    std::vector<S32> generateCodeChips(S32 prn) const override;
    std::vector<S32> generateSampledCode
        (const SampleParameter& sampleParam, S32 prn) const override;
};

#endif // GPS_L1CA_H