#include "simulator/fixedPoint.h"
#include <iostream>

QFormat FixedPoint::maxWidth(const QFormat& other) const {
    U32 maxIntWidth = qFormat_.integerWidth > other.integerWidth ? 
        qFormat_.integerWidth : other.integerWidth;
    U32 maxFracWidth = qFormat_.fractionWidth > other.fractionWidth ? 
        qFormat_.fractionWidth : other.fractionWidth;
    return QFormat(maxIntWidth, maxFracWidth);
}

void FixedPoint::showQFormat(void) const {
    std::cout << "Q" 
        << qFormat_.integerWidth 
        << "." 
        << qFormat_.fractionWidth 
        << std::endl;
}
void FixedPoint::showValues(void) const {
    std::cout 
        << "        value: "
        << value_
        << std::endl
        << "expandedValue: "
        << expandedValue_
        << std::endl;
}

FixedPoint::FixedPoint(const QFormat& qFormat, S64 value) : 
    qFormat_(qFormat), value_(value) {
    if ((qFormat.integerWidth == 0) ||
        (qFormat.integerWidth > 64) ||
        (qFormat.fractionWidth > 64 - qFormat.integerWidth)) {
        throw std::invalid_argument("FixedPoint: Invalid QFormat");
    }
    U32 totalWidth = qFormat.integerWidth + qFormat.fractionWidth;
    if (value & (S64)(~(((S128)1 << totalWidth) - 1))) {
        throw std::overflow_error("FixedPoint: Value overflowed with QFormat");
    }

    // expand sign bit
    if ((totalWidth < 64) && (value >> (totalWidth - 1))) {
        value |= ~(((S64)1 << totalWidth) - 1);
    }
    expandedValue_ = value;
    // check value
    // [-(2^x), 2^x) , x = total width - 1
    S128 limit = (S128)1 << (totalWidth - 1);
    if ((value >= limit) || (value < -limit)) {
        throw std::overflow_error("FixedPoint: Invalid value");
    }
}

S64 TruncateTo64Bits(S128 value, QFormat& qFormat) {
    // overflow
    if (qFormat.integerWidth > 64) {
        throw std::overflow_error("Integer overflowed, cannot truncate to 64 bits");
    }
    // no need to truncate
    if (qFormat.integerWidth + qFormat.fractionWidth <= 64) {
        return (S64)value;
    }
    // truncate fraction
    U32 reduntantBits = qFormat.integerWidth + qFormat.fractionWidth - 64;
    std::cout << "Warning: result overflowed, truncating to 64 bits: "
        << "Q" << qFormat.integerWidth << "." << qFormat.fractionWidth << " -> Q" 
        << qFormat.integerWidth << "." << 64 - qFormat.integerWidth
        << std::endl;
    qFormat.fractionWidth = 64 - qFormat.integerWidth;
    return (S64)(value >> reduntantBits);
}

double FixedToDouble(const FixedPoint& fp) {
    // obviously, fraction width of every fp <= 63
    QFormat q = fp.getQFormat();

    /*  [integer].[fraction]
            I         F
    
        [integer] + [fraction] / (2^F)
    */
    // integer
    double res = fp.getExpandedValue() >> q.fractionWidth;
    // fraction
    res += (double)(fp.getValue() & ((1 << q.fractionWidth) - 1)) / 
        (1 << q.fractionWidth);
    return res;
}


FixedPoint FixedPoint::operator+(const FixedPoint& other) const {
    QFormat tempFormat = maxWidth(other.getQFormat());
    if (tempFormat.integerWidth >= 64) {
        throw std::overflow_error("Integer overflowed");
    }
    // calculate with expanded value
    S128 tempResult = 
        ((S128)expandedValue_ << (tempFormat.fractionWidth - qFormat_.fractionWidth)) + 
        ((S128)other.getExpandedValue() << (tempFormat.fractionWidth - other.getQFormat().fractionWidth));
    tempFormat.integerWidth++;
    // drop the expanded sign bits
    if (tempFormat.integerWidth + tempFormat.fractionWidth < 128) {
        tempResult &= ((S128)1 << (tempFormat.integerWidth + tempFormat.fractionWidth)) - 1;
    }

    // possible truncate
    S64 truncatedResult = TruncateTo64Bits(tempResult, tempFormat);
    return FixedPoint(tempFormat, truncatedResult);
}

FixedPoint FixedPoint::operator-(const FixedPoint& other) const {
    QFormat tempFormat = maxWidth(other.getQFormat());
    if (tempFormat.integerWidth >= 64) {
        throw std::overflow_error("Integer overflowed");
    }
    // calculate with expanded value
    S128 tempResult = 
        ((S128)expandedValue_ << (tempFormat.fractionWidth - qFormat_.fractionWidth)) - 
        ((S128)other.getExpandedValue() << (tempFormat.fractionWidth - other.getQFormat().fractionWidth));
    tempFormat.integerWidth++;
    // drop the expanded sign bits
    if (tempFormat.integerWidth + tempFormat.fractionWidth < 128) {
        tempResult &= ((S128)1 << (tempFormat.integerWidth + tempFormat.fractionWidth)) - 1;
    }

    // possible truncate
    S64 truncatedResult = TruncateTo64Bits(tempResult, tempFormat);
    return FixedPoint(tempFormat, truncatedResult);
}

FixedPoint FixedPoint::operator*(const FixedPoint& other) const {
    QFormat tempFormat = {qFormat_.integerWidth + other.getQFormat().integerWidth, \
        qFormat_.fractionWidth + other.getQFormat().fractionWidth};
    if (tempFormat.integerWidth > 64) {
        throw std::overflow_error("Integer overflowed");
    }
    S128 tempResult = (S128)expandedValue_ * (S128)other.getExpandedValue();
    // drop the expanded sign bits
    if (tempFormat.fractionWidth < 128 - tempFormat.integerWidth) {
        tempResult &= ((S128)1 << (tempFormat.integerWidth + tempFormat.fractionWidth)) - 1;
    }

    // possible truncate
    S64 truncatedResult = TruncateTo64Bits(tempResult, tempFormat);
    return FixedPoint(tempFormat, truncatedResult);
}