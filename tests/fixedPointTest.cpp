#include "simulator/fixedPoint.h"
#include <gtest/gtest.h>
#include <math.h>

TEST(QFormatTest, Constructor) {
    QFormat q(3, 5);
    ASSERT_EQ(q.integerWidth, 3);
    ASSERT_EQ(q.fractionWidth, 5);
}

TEST(FixedPointTest, maxWidth) {
    FixedPoint a(QFormat(1, 4), 0);
    FixedPoint b(QFormat(3, 2), 0);
    QFormat maxQ = a.maxWidth(b.getQFormat());
    ASSERT_EQ(maxQ.integerWidth, 3);
    ASSERT_EQ(maxQ.fractionWidth, 4);
}

TEST(FixedPointTest, Constructor_Invalid) {
    // Invalid QFormat
    ASSERT_THROW(FixedPoint(QFormat(0, 1), 0), std::invalid_argument);
    ASSERT_THROW(FixedPoint(QFormat(65, 0), 0), std::invalid_argument);
    ASSERT_THROW(FixedPoint(QFormat(32, 33), 0), std::invalid_argument);

    // Invalid value
    ASSERT_THROW(FixedPoint(QFormat(3, 5), (1LL << (3 + 5))), std::overflow_error);
}

TEST(FixedPointTest, Constructor_Valid) {
    FixedPoint fp(QFormat(2, 2), 0);
    ASSERT_EQ(fp.getQFormat().integerWidth, 2);
    ASSERT_EQ(fp.getQFormat().fractionWidth, 2);
    ASSERT_EQ(fp.getValue(), 0);
}

TEST(FixedPointTest, TruncateTo64Bits_Invalid) {
    QFormat q(65, 0);
    ASSERT_THROW(TruncateTo64Bits(0, q), std::overflow_error);
}

TEST(FixedPointTest, TruncateTo64Bits_LessThan64Bits) {
    QFormat a(32, 32);
    ASSERT_EQ(TruncateTo64Bits(123, a), 123);

    QFormat b(12, 34);
    ASSERT_EQ(TruncateTo64Bits(321, b), 321);
}

TEST(FixedPointTest, TruncateTo64Bits_MoreThan64Bits_1) {
    QFormat q(32, 33);
    S128 value = (S128)1 << 33; // represents 1
    S64 truncatedValue = TruncateTo64Bits(value, q);
    ASSERT_EQ(q.fractionWidth, 32); // fraction width should be 32
    ASSERT_EQ(truncatedValue, (S64)(value >> 1)); // right shifted 1 bit
}


TEST(FixedPointTest, TruncateTo64Bits_MoreThan64Bits_2) {
    QFormat q(64, 10);
    S128 value = ((S128)1 << 10) + 1; // represents 1 + 2^(-10)
    S64 truncatedValue = TruncateTo64Bits(value, q);
    ASSERT_EQ(q.fractionWidth, 0); // fraction width should be 0
    ASSERT_EQ(truncatedValue, (S64)(value >> 10)); // right shifted 10 bit
}

TEST(FixedPointTest, FixedToDouble) {
    /*
        0b11010, Q3.2
        110 -> -4 + 2 = -2
        10 -> 2^(-1) = 0.5
        fp: -1.5
    */
    FixedPoint fp(QFormat(3, 2), 0b11010);
    double res = FixedToDouble(fp);
    ASSERT_TRUE(F_EQ(res, -1.5));
}

TEST(FixedPointTest, Add_Invalid) {
    FixedPoint a(QFormat(32, 32),  0);
    FixedPoint b(QFormat(64, 0), 0);
    ASSERT_THROW(FixedPoint c = a + b, std::overflow_error);
}

TEST(FixedPointTest, Add1) {
    FixedPoint a(QFormat(2, 2),  0b0100); // 1.0
    FixedPoint b(QFormat(3, 2), 0b01000); // 2.0
    FixedPoint c = a + b;
    ASSERT_EQ(c.getQFormat().integerWidth, 4);
    ASSERT_EQ(c.getQFormat().fractionWidth, 2);
    ASSERT_EQ(c.getValue(), 0b001100); // 3.0
}

TEST(FixedPointTest, Add2) {
    FixedPoint a(QFormat(2, 2),  0b1110); // -0.5
    FixedPoint b(QFormat(3, 3), 0b010010); // 2.25
    FixedPoint c = a + b;
    ASSERT_EQ(c.getQFormat().integerWidth, 4);
    ASSERT_EQ(c.getQFormat().fractionWidth, 3);
    ASSERT_EQ(c.getValue(), 0b0001110); // 1.75
}

TEST(FixedPointTest, Add3) {
    FixedPoint a(QFormat(63, 1), ((S64)1 << 63) + 1); // -(2^62) + 0.5
    FixedPoint b(QFormat(1, 63), ((S64)1 << 63) + 1); // -1 + 2^(-63)
    FixedPoint c = a + b;
    ASSERT_EQ(c.getQFormat().integerWidth, 64);
    ASSERT_EQ(c.getQFormat().fractionWidth, 0);
    ASSERT_EQ(c.getValue(), ~((S64)1 << 62)); // -(2^62) - 1 -> 0b1011...1
    ASSERT_EQ(c.getValue(), -((S128)65536 * 65536 * 65536 * 16384) - 1); // -(2^62) - 1
}

TEST(FixedPointTest, Sub_Invalid) {
    FixedPoint a(QFormat(32, 32),  0);
    FixedPoint b(QFormat(64, 0), 0);
    ASSERT_THROW(FixedPoint c = a - b, std::overflow_error);
}

TEST(FixedPointTest, Sub1) {
    FixedPoint a(QFormat(2, 2),  0b0100); // 1.0
    FixedPoint b(QFormat(3, 2), 0b01000); // 2.0
    FixedPoint c = a - b;
    ASSERT_EQ(c.getQFormat().integerWidth, 4);
    ASSERT_EQ(c.getQFormat().fractionWidth, 2);
    ASSERT_EQ(c.getValue(), 0b111100); // -1.0
}

TEST(FixedPointTest, Sub2) {
    FixedPoint a(QFormat(2, 2),  0b1110); // -0.5
    FixedPoint b(QFormat(3, 3), 0b010010); // 2.25
    FixedPoint c = a - b;
    ASSERT_EQ(c.getQFormat().integerWidth, 4);
    ASSERT_EQ(c.getQFormat().fractionWidth, 3);
    ASSERT_EQ(c.getValue(), 0b1101010); // -2.75
}

TEST(FixedPointTest, Sub3) {
    FixedPoint a(QFormat(63, 1), ((S64)1 << 63) + 1); // -(2^62) + 0.5
    FixedPoint b(QFormat(1, 63), ((S64)1 << 63) + 1); // -1 + 2^(-63)
    FixedPoint c = a - b;
    ASSERT_EQ(c.getQFormat().integerWidth, 64);
    ASSERT_EQ(c.getQFormat().fractionWidth, 0);
    ASSERT_EQ(c.getValue(), ((S64)3 << 62) + 1); // -(2^62) + 1 -> 0b1100...01
    ASSERT_EQ(c.getValue(), -((S128)65536 * 65536 * 65536 * 16384) + 1); // -(2^62) + 1
}

TEST(FixedPointTest, Mul_Invalid) {
    FixedPoint a(QFormat(32, 32), 0);
    FixedPoint b(QFormat(33, 31), 0);
    ASSERT_THROW(FixedPoint c = a * b, std::overflow_error);
}

TEST(FixedPointTest, Mul1) {
    FixedPoint a(QFormat(2, 2),  0b0100); // 1.0
    FixedPoint b(QFormat(3, 2), 0b01000); // 2.0
    FixedPoint c = a * b;
    ASSERT_EQ(c.getQFormat().integerWidth, 5);
    ASSERT_EQ(c.getQFormat().fractionWidth, 4);
    ASSERT_EQ(c.getValue(), 0b000100000); // 2.0
}

TEST(FixedPointTest, Mul2) {
    FixedPoint a(QFormat(2, 2),  0b1110); // -0.5
    FixedPoint b(QFormat(3, 3), 0b010010); // 2.25
    FixedPoint c = a * b;
    ASSERT_EQ(c.getQFormat().integerWidth, 5);
    ASSERT_EQ(c.getQFormat().fractionWidth, 5);
    ASSERT_EQ(c.getValue(), 0b1111011100); // -1.125
    // -2 + 0.5 + 0.25 + 0.125
    // -16 + 8 + 4 + 2
}

TEST(FixedPointTest, Mul3) {
    std::cout << "((S64)1 << 63) + 1 = " << ((S64)1 << 63) + 1 << std::endl;
    FixedPoint a(QFormat(63, 1), ((S64)1 << 63) + 1); // -(2^62) + 0.5
    FixedPoint b(QFormat(1, 63), ((S64)1 << 63) + 1); // -1 + 2^(-63)
    FixedPoint c = a * b;
    ASSERT_EQ(c.getQFormat().integerWidth, 64);
    ASSERT_EQ(c.getQFormat().fractionWidth, 0);
    /*
        result = 2^62 - 0.5 - 2^(-1) + 2^(-64) = 2^62 - 1 + 2^(-64)
            ~ 2^62 - 1
            0b 0011...1
    */
    ASSERT_EQ(c.getValue(), ((S64)1 << 62) - 1);
    ASSERT_EQ(c.getValue(), ((S128)65536 * 65536 * 65536 * 16384) - 1);
}