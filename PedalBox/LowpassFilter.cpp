#include "LowpassFilter.h"

LowpassFilter::LowpassFilter() {
    z1 = z2 = 0.0;
}

LowpassFilter::LowpassFilter(float Fc, float Q, float peakGainDB) {
    setLowpassFilter(Fc, Q, peakGainDB);
}

LowpassFilter::~LowpassFilter() {
}

/**
 * Sets the frequency
 * Calculate as Fc = f/samplerate
 * Must be lower than 0.5
 */
void LowpassFilter::setFc(float Fc) {
    Fc = constrain(Fc, 0, 0.5);
    this->Fc = Fc;
    calcLowpassFilter();
}

/**
 * Changes Q value and recalculaes filter
 */
void LowpassFilter::setQ(float Q) {
    this->Q = Q;
    calcLowpassFilter();
}

/**
 * Calculates one step of the filter and returns the output
 */
float LowpassFilter::process(float in) {
    float out = in * a0 + z1;
    z1 = in * a1 + z2 - b1 * out;
    z2 = in * a2 - b2 * out;
    return out;
}

void LowpassFilter::setLowpassFilter(float Fc, float Q, float peakGainDB) {
    Fc = constrain(Fc, 0, 0.5);
    this->Q = Q;
    this->Fc = Fc;
    this->peakGain = peakGainDB;
    calcLowpassFilter();
}

/*
 * Updates parameters and resets the LowpassFilter filter
 */
void LowpassFilter::calcLowpassFilter(void) {
    z1 = 0.0;
    z2 = 0.0;
    float norm;
    float V = pow(10, fabs(peakGain) / 20.0);
    float K = tan(M_PI * Fc);

    // lowpass
    norm = 1 / (1 + K / Q + K * K);
    a0 = K * K * norm;
    a1 = 2 * a0;
    a2 = a0;
    b1 = 2 * (K * K - 1) * norm;
    b2 = (1 - K / Q + K * K) * norm;

    return;
}
