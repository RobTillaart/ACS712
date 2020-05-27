//
//    FILE: ACS712.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.3
//    DATE: 2020-03-17
// PURPOSE: ACS712 library - current measurement
//
// HISTORY:
// 0.1.0    2020-03-17 initial version
// 0.1.1    2020-03-18 first release version
// 0.1.2	2020-03-21 automatic formfactor test
// 0.1.3    2020-05-27 fix library.json
//

#include "ACS712.h"

ACS712::ACS712(uint8_t analogPin, float volts, uint16_t maxADC, uint8_t mVperA)
{
    _pin = analogPin;
    // 1x 1000 for V -> mV
    _mVpstep = 1000.0 * volts / maxADC;
    _mVperAmpere = mVperA;
    _formFactor = 0.70710678119;  // 0.5 * sqrt(2);  TODO: should be smaller in practice 0.5 ?
    _midPoint = maxADC / 2;
}

int ACS712::mA_AC(uint8_t freq)
{
    uint32_t start = micros();
    uint16_t period = ((freq == 60) ? 16670 : 20000);
    uint8_t samples = 0;
    uint8_t zeros = 0;
    int _min, _max;
    _min = _max = analogRead(_pin);
    while (micros() - start < period)  // UNO ~180 samples...
    {
        samples++;
        int val = analogRead(_pin);
        if (val < _min) _min = val;
        if (val > _max) _max = val;
        if (abs(val - _midPoint) < 4) zeros++;   // Noise max 4 during test
    }
    int p2p = (_max - _min);

    // automatic determine _formFactor / crest factor   
    float D = 0;	
    float FF = 0;
    if (zeros > 5)                          // MAGIC NUMBER 5 ~2.5% of samples = zero
    {
      D = 1.0 - (1.0 * zeros) / samples;    // % SAMPLES NONE ZERO
      FF = sqrt(D) * 0.5 * sqrt(2);         // ASSUME NON ZERO PART ~ SINUS
	}
	else  // # zeros is small => D --> 1 --> sqrt(D) --> 1
	{
      FF = 0.5 * sqrt(2);
    }
    _formFactor = FF;

    // math could be partially precalculated: C = 1000.0 * 0.5 * _mVpstep / _mVperAmpere;
    // rounding?
    return 1000.0 * 0.5 * p2p * _mVpstep * _formFactor / _mVperAmpere;
}

int ACS712::mA_DC()
{
    // read twice to stabilize...
    analogRead(_pin);
    int steps = analogRead(_pin) - _midPoint;
    return 1000.0 * steps * _mVpstep / _mVperAmpere;
}

// END OF FILE
