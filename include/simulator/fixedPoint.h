#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

#include <iostream>
#include "common.h"

class QFormat {
public:
    U32 integerWidth;
    U32 fractionWidth;
    QFormat(U32 integerWidth, U32 fractionWidth) : 
        integerWidth(integerWidth), fractionWidth(fractionWidth) {}
};

class FixedPoint {
private:
    QFormat qFormat_;
    S64     value_;         // fixed point
    S64     expandedValue_;   // fixed point with expanded sign bit
public:
    FixedPoint(const QFormat& qFormat, S64 value);
    FixedPoint(const QFormat& qFormat) : FixedPoint(qFormat, 0) {}
    
    QFormat getQFormat() const {
        return qFormat_;
    }
    S64 getValue() const {
        return value_;
    }
    S64 getExpandedValue() const {
        return expandedValue_;
    }
    void showQFormat(void) const;
    void showValues(void) const;
    
    // get the max width (both integer and fraction) between input and current QFormat
    QFormat maxWidth(const QFormat& other) const;

    // operator overload
    FixedPoint operator+(const FixedPoint& other) const;
    FixedPoint operator-(const FixedPoint& other) const;
    FixedPoint operator-(void) const;
    FixedPoint operator*(const FixedPoint& other) const;
};

/* 
    truncate the value and qFormat from 128 bits to 64 bits
    input:  original 128bit value and its qFormat
    output: truncated qFormat
    return: truncated 64bit value
*/ 
S64 TruncateTo64Bits(S128 value, QFormat& qFormat);

// convert fixed point to float point (double)
double FixedToDouble(const FixedPoint& fp);

#endif // FIXEDPOINT_H