# Acquisition

I verify the acquisition algorithm in both float and fixed data input. The raw input data is from the "IF data generator" developed with claude, which produce data with width of 2 bits in both I and Q, like the open-source hardware PocketSDR.

---

## FFT module

Radix-2 DFT. User should provide the address, length and type of the input data. The length should be the power of 2. This module will return the FFT result by in-situ modification.

This module has both float and fixed point versions.

### From DFT to FFT

The DFT of time domain sequence $x[n]$ in frequency index $k$ is

$$
X(k) = \sum_0^{N - 1} x[n] e^{-j 2\pi k \frac nN} = \sum_0^{N - 1} x[n] W_N^{nk}
$$

where $W_N = e^{-j \frac {2\pi}N}$ is the twiddle factor.

---

