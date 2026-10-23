#ifndef FFT_H
#define FFT_H

#include <cmath>
#include <typeinfo>
#include "common.h"
#include "simulator/complex.h"

#define FFT_SUCCESS 0
#define FFT_INVALID_INPUT 1

template <typename T>
Complex<T> GetWiddleFactor(U32 k, U32 N) {
    // float point
    if (typeid(T) == typeid(double)) {
        double angle = -2.0 * M_PI * k / N;
        return Complex<T>(cos(angle), sin(angle));
    } else {
    // fixed point
    // TODO
        return Complex<T>(0, 0);
    }
}

// in situ calculation
template <typename T>
U32 fft(Complex<T> *data, U32 length) {
    // length must be a power of 2
    if ((!data) || (length == 0) || ((length & (length - 1)) != 0)) {
        return FFT_INVALID_INPUT;
    }

    // bit reversal
    U32 *index = new U32[length];
    for (U32 i = 0; i < length; i++) {
        U32 temp = i;
        // reverse all bits of i
        temp = ((temp & 0xaaaaaaaa) >> 1) | ((temp & 0x55555555) << 1);
        temp = ((temp & 0xcccccccc) >> 2) | ((temp & 0x33333333) << 2);
        temp = ((temp & 0xf0f0f0f0) >> 4) | ((temp & 0x0f0f0f0f) << 4);
        temp = ((temp & 0xff00ff00) >> 8) | ((temp & 0x00ff00ff) << 8);
        temp = (temp >> 16) | (temp << 16);
        index[i] = temp >> (32 - __builtin_ctz(length));
    }
    Complex<T> *reversed_data = new Complex<T>[length];
    for (U32 i = 0; i < length; i++) {
        reversed_data[i] = data[index[i]];
    }
    delete[] index;

    // fft
    for (U32 step = 2; step <= length; step <<= 1) {
        for (U32 i = 0; i < length; i += step) {
            // butterfly
            for (U32 j = 0; j < step / 2; j++) {
                Complex<T> widdleFactor = GetWiddleFactor<T>(j, step);
                Complex<T> u = reversed_data[i + j];
                Complex<T> v = reversed_data[i + j + step / 2] * widdleFactor;
                reversed_data[i + j] = u + v;
                reversed_data[i + j + step / 2] = u - v;
            }
        }
    }

    // save
    for (U32 i = 0; i < length; ++i) {
        data[i] = reversed_data[i];
    }
    delete[] reversed_data;

    return FFT_SUCCESS;
}

// in situ calculation
template <typename T>
U32 ifft(Complex<T> *data, U32 length) {
    // length must be a power of 2
    if ((!data) || (length == 0) || ((length & (length - 1)) != 0)) {
        return FFT_INVALID_INPUT;
    }

    // conjugate
    for (U32 i = 0; i < length; ++i) {
        data[i] = data[i].conjugate();
    }

    // fft
    (void)fft(data, length);
    
    // conjugate and normalization
    for (U32 i = 0; i < length; ++i) {
        data[i] = data[i].conjugate();
        data[i].real /= length;
        data[i].imag /= length;
    }

    return FFT_SUCCESS;
}
#endif // FFT_H