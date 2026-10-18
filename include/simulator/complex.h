#ifndef COMPLEX_H
#define COMPLEX_H

template <typename T>
class Complex {
public: 
    T real;
    T imag;

    Complex(T r = 0, T i = 0) : real(r), imag(i) {}

    Complex<T> operator+(const Complex<T>& other) const {
        return Complex<T>(real + other.real, imag + other.imag);
    }

    Complex<T> operator-(const Complex<T>& other) const {
        return Complex<T>(real - other.real, imag - other.imag);
    }

    Complex<T> operator*(const Complex<T>& other) const {
        return Complex<T>(real * other.real - imag * other.imag,
                          real * other.imag + imag * other.real);
    }
};

#endif // COMPLEX_H