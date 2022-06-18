/*
 * Filters.cpp
 *
 *  Created on: Feb 13, 2020
 *      Author: Yannick
 */

#include <math.h>

enum class BiquadType : uint8_t {
    lowpass = 0,
    highpass,
    bandpass,
    notch,
    peak,
    lowshelf,
    highshelf
};

class Biquad{
public:
  Biquad();
    Biquad(BiquadType type, float Fc, float Q, float peakGainDB);
    ~Biquad();
    float process(float in);
    void setBiquad(BiquadType type, float Fc, float Q, float peakGain);
    void setFc(float Fc); //frequency
    void setQ(float Q);
    void calcBiquad(void);

protected:

    BiquadType type;
    float a0, a1, a2, b1, b2;
    float Fc, Q, peakGain;
    float z1, z2;
};


Biquad::Biquad(){
	z1 = z2 = 0.0;
}
Biquad::Biquad(BiquadType type, float Fc, float Q, float peakGainDB) {
    setBiquad(type, Fc, Q, peakGainDB);
}

Biquad::~Biquad() {
}

/**
 * Sets the frequency
 * Calculate as Fc = f/samplerate
 * Must be lower than 0.5
 */
void Biquad::setFc(float Fc) {
	Fc = constrain(Fc,0,0.5);
    this->Fc = Fc;
    calcBiquad();
}

/**
 * Changes Q value and recalculaes filter
 */
void Biquad::setQ(float Q) {
    this->Q = Q;
    calcBiquad();
}

/**
 * Calculates one step of the filter and returns the output
 */
float Biquad::process(float in) {
	float out = in * a0 + z1;
    z1 = in * a1 + z2 - b1 * out;
    z2 = in * a2 - b2 * out;
    return out;
}

void Biquad::setBiquad(BiquadType type, float Fc, float Q, float peakGainDB) {
	Fc = constrain(Fc,0,0.5);
    this->type = type;
    this->Q = Q;
    this->Fc = Fc;
    this->peakGain = peakGainDB;
    calcBiquad();
}

/*
 * Updates parameters and resets the biquad filter
 */
void Biquad::calcBiquad(void) {
	z1 = 0.0;
	z2 = 0.0;
    float norm;
    float V = pow(10, fabs(peakGain) / 20.0);
    float K = tan(M_PI * Fc);
    switch (this->type) {
        case BiquadType::lowpass:
            norm = 1 / (1 + K / Q + K * K);
            a0 = K * K * norm;
            a1 = 2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;
    }

    return;
}
