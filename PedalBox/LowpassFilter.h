#include "Arduino.h"
#include <math.h>

class LowpassFilter{
    public:
        LowpassFilter();
        LowpassFilter(float Fc, float Q, float peakGainDB);
        ~LowpassFilter();
        float process(float in);
        void setLowpassFilter(float Fc, float Q, float peakGain);
        void setFc(float Fc); //frequency
        void setQ(float Q);
        void calcLowpassFilter(void);

    protected:
        float a0, a1, a2, b1, b2;
        float Fc, Q, peakGain;
        float z1, z2;
};
