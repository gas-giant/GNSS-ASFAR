#include "common.h"
#include "simulator/signals/gps_l1ca.h"
#include <vector>

// From PRN number to bit index in register G2
// PRN --> [t1, t2]
// Source: IS-GPS-200N Table 3-Ia "Code Phase Assignments", PRN 1-32.
static const S32 g_g2PrnTable[32][2] = {
    {2, 6},  {3, 7},  {4, 8},  {5, 9},  {1, 9},  {2, 10}, {1, 8},  {2, 9},
    {3, 10}, {2, 3},  {3, 4},  {5, 6},  {6, 7},  {7, 8},  {8, 9},  {9, 10},
    {1, 4},  {2, 5},  {3, 6},  {4, 7},  {5, 8},  {6, 9},  {1, 3},  {4, 6},
    {5, 7},  {6, 8},  {7, 9},  {8, 10}, {1, 6},  {2, 7},  {3, 8},  {4, 9},
};

// Generate GPS L1 C/A code chips. Value in {-1, 1}.
std::vector<S32> SignalGpsL1CA::generateCodeChips(S32 prn) {
    if (prn < 1 || prn > 32) {
        throw std::invalid_argument(
            "L1 C/A PRN out of range (GPS 1..32, QZSS 193..202): " +
            std::to_string(prn));
    }
    // Shift bit register G1 and G2, initial bits are 1
    // G1 polynomial: X^10 + X^3 + 1
    // G2 polynomial: X^10 + X^9 + X^8 + X^6 + X^3 + X^2 + 1
    S32 g1[10];
    S32 g2[10];
    for (S32 i = 0; i < 10; ++i) {
        g1[i] = g2[i] = 1;
    }

    // from PRN number to G2 register bit index [t1, t2]
    S32 t1 = g_g2PrnTable[prn - 1][0] - 1;
    S32 t2 = g_g2PrnTable[prn - 1][1] - 1;
    
    std::vector<S32> code(1023);
    for (S32 i = 0; i < 1023; ++i) {
        // current code chip
        /*
            The i-th code chip (total 1023 chips)
    
            G1 --[bit 10]-->   \
                                \
            G2 --[bit t1]--> -- xor --> code chip [i]
               --[bit t2]-->    /
        */
        S32 bit = g1[9] ^ g2[t1] ^ g2[t2];
        /*  BPSK converting
            0 -> +1
            1 -> -1
        */
        code[i] = bit ? -1 : +1;

        // update register G1 and G2
        // new bit[0]
        S32 nb1 = g1[2] ^ g1[9];
        S32 nb2 = g2[1] ^ g2[2] ^ g2[5] ^ g2[7] ^ g2[8] ^ g2[9];
        // shift move bits of register
        for (S32 j = 9; j > 0; --j) {
            g1[j] = g1[j - 1];
            g2[j] = g2[j - 1];
        }
        g1[0] = nb1;
        g2[0] = nb2;
    }
    return code;
}

/*
    input:  fs
            fc                  frequency of code (Hz)
            fs                  frequency of sampling (Hz)
            prn                 PRN number
            fdCode              Doppler frequency of code (Hz)
            initialCodePhase    initial code phase (chip)
*/
std::vector<S32> SignalGpsL1CA::generateSampledCode
    (const SampleParameter& sampleParam, S32 prn) {
    if (prn < 1 || prn > 32) {
        throw std::invalid_argument(
            "L1 C/A PRN out of range (GPS 1..32, QZSS 193..202): " +
            std::to_string(prn));
    }
    // code chips
    std::vector<S32> codeChips = generateCodeChips(prn);

    // sampling
    double step = (fCode_ + sampleParam.fdCode) / sampleParam.fs;
    std::vector<S32> res(sampleParam.length);
    for (U32 i = 0; i < sampleParam.length; ++i) {
        res[i] = codeChips[
            static_cast<S32>(std::floor(sampleParam.initialCodePhase + i * step)) % 1023
        ];
    }
    return res;
}