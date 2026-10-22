#include "simulator/complex.h"
#include "simulator/fixedPoint.h"
#include <gtest/gtest.h>

TEST(ComplexTest, Constructor) {
    FixedPoint re(QFormat(2, 2), 0b0110); // 1.5
    FixedPoint im(QFormat(2, 2), 0b1011); // -1.25
    Complex<FixedPoint> x(re, im);
    ASSERT_TRUE(F_EQ(FixedToDouble(x.real), 1.5));
    ASSERT_TRUE(F_EQ(FixedToDouble(x.imag), -1.25));
}

TEST(ComplexTest, Add) {
    Complex<FixedPoint> a(
        FixedPoint(QFormat(2, 1), 0b011),   // 1.5
        FixedPoint(QFormat(3, 2), 0b11011)  // -4 + 2 + 0.5 + 0.25 = -1.25
    );
    Complex<FixedPoint> b(
        FixedPoint(QFormat(1, 3), 0b0110),  // 0.75
        FixedPoint(QFormat(4, 0), 0b1011)   // -5
    );
    Complex<FixedPoint> c = a + b;
    ASSERT_EQ(c.real.getQFormat().integerWidth, 3);
    ASSERT_EQ(c.real.getQFormat().fractionWidth, 3);
    ASSERT_EQ(c.imag.getQFormat().integerWidth, 5);
    ASSERT_EQ(c.imag.getQFormat().fractionWidth, 2);
    ASSERT_TRUE(F_EQ(FixedToDouble(c.real), 2.25));
    ASSERT_TRUE(F_EQ(FixedToDouble(c.imag), -6.25));
}

TEST(ComplexTest, Sub) {
    Complex<FixedPoint> a(
        FixedPoint(QFormat(2, 1), 0b011),   // 1.5
        FixedPoint(QFormat(3, 2), 0b11011)  // -4 + 2 + 0.5 + 0.25 = -1.25
    );
    Complex<FixedPoint> b(
        FixedPoint(QFormat(1, 3), 0b0110),  // 0.75
        FixedPoint(QFormat(4, 0), 0b1011)   // -5
    );
    Complex<FixedPoint> c = a - b;
    ASSERT_EQ(c.real.getQFormat().integerWidth, 3);
    ASSERT_EQ(c.real.getQFormat().fractionWidth, 3);
    ASSERT_EQ(c.imag.getQFormat().integerWidth, 5);
    ASSERT_EQ(c.imag.getQFormat().fractionWidth, 2);
    ASSERT_TRUE(F_EQ(FixedToDouble(c.real), 0.75));
    ASSERT_TRUE(F_EQ(FixedToDouble(c.imag), 3.75));
}

TEST(ComplexTest, Mul) {
    Complex<FixedPoint> a(
        FixedPoint(QFormat(2, 1), 0b011),   // 1.5
        FixedPoint(QFormat(3, 2), 0b11011)  // -4 + 2 + 0.5 + 0.25 = -1.25
    );
    Complex<FixedPoint> b(
        FixedPoint(QFormat(1, 3), 0b0110),  // 0.75
        FixedPoint(QFormat(4, 0), 0b1011)   // -5
    );
    Complex<FixedPoint> c = a * b;
    /*
        r * r -> Q3.4 -> 1.125
        i * i -> Q7.2 -> 6.25
        r * i -> Q6.1 -> -7.5
        i * r -> Q4.5 -> -0.9375
    */
    ASSERT_EQ(c.real.getQFormat().integerWidth, 8);
    ASSERT_EQ(c.real.getQFormat().fractionWidth, 4);
    ASSERT_EQ(c.imag.getQFormat().integerWidth, 7);
    ASSERT_EQ(c.imag.getQFormat().fractionWidth, 5);
    ASSERT_TRUE(F_EQ(FixedToDouble(c.real), -5.125));
    ASSERT_TRUE(F_EQ(FixedToDouble(c.imag), -8.4375));
}

TEST(ComplexTest, Conj) {
    Complex<FixedPoint> a(
        FixedPoint(QFormat(2, 1), 0b011),   // 1.5
        FixedPoint(QFormat(3, 2), 0b11011)  // -4 + 2 + 0.5 + 0.25 = -1.25
    );
    Complex<FixedPoint> b(
        FixedPoint(QFormat(1, 3), 0b0110),  // 0.75
        FixedPoint(QFormat(4, 0), 0b1011)   // -5
    );
    ASSERT_TRUE(F_EQ(FixedToDouble(a.conjugate().real), 1.5));
    ASSERT_TRUE(F_EQ(FixedToDouble(a.conjugate().imag), 1.25));
    ASSERT_TRUE(F_EQ(FixedToDouble(b.conjugate().real), 0.75));
    ASSERT_TRUE(F_EQ(FixedToDouble(b.conjugate().imag), 5));
}

TEST(ComplexTest, Neg) {
    Complex<FixedPoint> a(
        FixedPoint(QFormat(2, 1), 0b011),   // 1.5
        FixedPoint(QFormat(3, 2), 0b11011)  // -4 + 2 + 0.5 + 0.25 = -1.25
    );
    Complex<FixedPoint> b(
        FixedPoint(QFormat(1, 3), 0b0110),  // 0.75
        FixedPoint(QFormat(4, 0), 0b1011)   // -5
    );
    ASSERT_TRUE(F_EQ(FixedToDouble((-a).real), -1.5));
    ASSERT_TRUE(F_EQ(FixedToDouble((-a).imag), 1.25));
    ASSERT_TRUE(F_EQ(FixedToDouble((-b).real), -0.75));
    ASSERT_TRUE(F_EQ(FixedToDouble((-b).imag), 5));
}