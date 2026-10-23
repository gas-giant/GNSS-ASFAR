#include "simulator/complex.h"
#include "simulator/fixedPoint.h"
#include "simulator/acquisition/fft.h"
#include <gtest/gtest.h>

TEST(fftTest, Invalid) {
    ASSERT_EQ(fft<double>(nullptr, 1), FFT_INVALID_INPUT);
    Complex<double> a[] = {
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
    };
    ASSERT_EQ(fft(a, 0), FFT_INVALID_INPUT);
    ASSERT_EQ(fft(a, 3), FFT_INVALID_INPUT);
    ASSERT_EQ(fft(a, 5), FFT_INVALID_INPUT);
}

TEST(fftTest, double1) {
    Complex<double> a[] = {
        {1.25, -0.5},
    };
    ASSERT_EQ(fft(a, 1), FFT_SUCCESS);
    ASSERT_TRUE(F_EQ(a[0].real, 1.25));
    ASSERT_TRUE(F_EQ(a[0].imag, -0.5));
}

TEST(fftTest, double2) {
    Complex<double> a[] = {
        {1, 0},
        {2, 0},
        {3, 0},
        {4, 0},
    };
    ASSERT_EQ(fft(a, 4), FFT_SUCCESS);
    ASSERT_TRUE(F_EQ(a[0].real, 10));
    ASSERT_TRUE(F_EQ(a[0].imag, 0));
    ASSERT_TRUE(F_EQ(a[1].real, -2));
    ASSERT_TRUE(F_EQ(a[1].imag, 2));
    ASSERT_TRUE(F_EQ(a[2].real, -2));
    ASSERT_TRUE(F_EQ(a[2].imag, 0));
    ASSERT_TRUE(F_EQ(a[3].real, -2));
    ASSERT_TRUE(F_EQ(a[3].imag, -2));
}

TEST(fftTest, double3) {
    Complex<double> a[] = {
        {1, 0},
        {2, 0},
        {3, 0},
        {4, 0},
        {5, 0},
        {6, 0},
        {7, 0},
        {8, 0},
    };
    ASSERT_EQ(fft(a, 8), FFT_SUCCESS);
    ASSERT_TRUE(F_EQ(a[0].real, 36));
    ASSERT_TRUE(F_EQ(a[0].imag, 0));
    ASSERT_TRUE(F_EQ(a[1].real, -4));
    ASSERT_TRUE(F_EQ(a[1].imag, 9.65685425));
    ASSERT_TRUE(F_EQ(a[2].real, -4));
    ASSERT_TRUE(F_EQ(a[2].imag, 4));
    ASSERT_TRUE(F_EQ(a[3].real, -4));
    ASSERT_TRUE(F_EQ(a[3].imag, 1.65685425f));
    ASSERT_TRUE(F_EQ(a[4].real, -4));
    ASSERT_TRUE(F_EQ(a[4].imag, 0));
    ASSERT_TRUE(F_EQ(a[5].real, -4));
    ASSERT_TRUE(F_EQ(a[5].imag, -1.65685425f));
    ASSERT_TRUE(F_EQ(a[6].real, -4));
    ASSERT_TRUE(F_EQ(a[6].imag, -4));
    ASSERT_TRUE(F_EQ(a[7].real, -4));
    ASSERT_TRUE(F_EQ(a[7].imag, -9.65685425));
}
