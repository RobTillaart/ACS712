#pragma once
//
//    FILE: ACS712.h
//  AUTHOR: Rob Tillaart, Pete Thompson
// VERSION: 0.2.8
//    DATE: 2020-08-02
// PURPOSE: ACS712 library - current measurement
//
// Tested with a RobotDyn ACS712 20A breakout + UNO.
//


#include "Arduino.h"

#define ACS712_LIB_VERSION        (F("0.2.8"))


//  ACS712_FF_SINUS == 1.0/sqrt(2) == 0.5 * sqrt(2)
//  should be smaller in practice 0.5 ?
#define ACS712_FF_SINUS           (1.0/sqrt(2))
#define ACS712_FF_SQUARE          (1.0)
#define ACS712_FF_TRIANGLE        (1.0/sqrt(3))
#define ACS712_FF_SAWTOOTH        (1.0/sqrt(3))

#define ACS712_DEFAULT_FREQ       50
#define ACS712_DEFAULT_NOISE      21


class ACS712
{
  public:
    //  NOTE:
    //  One can quite precisely tune the value of the sensor
    //      (1) the milliVolt per Ampere and
    //      (2) the volts parameter.
    //
    //  TYPE   mV per Ampere
    //  5A        185.0
    //  20A       100.0
    //  30A        66.0
    ACS712(uint8_t analogPin, float volts = 5.0, uint16_t maxADC = 1023, float mVperAmpere = 100);


    //  returns mA
    //  blocks 20-21 ms to sample a whole 50 or 60 Hz period.
    //  works with point2point level and Form Factor.
    //  lower frequencies block longer.
    int      mA_AC(float frequency = ACS712_DEFAULT_FREQ, uint16_t cycles = 1);

    //  returns mA
    //  blocks 20-21 ms to sample a whole 50 or 60 Hz period.
    //  works with sampling.
    //  lower frequencies block longer.
    float    mA_AC_sampling(float frequency = ACS712_DEFAULT_FREQ, uint16_t cycles = 1);

    //  returns mA
    //  blocks < 1 ms
    int      mA_DC(uint16_t samples = 1);


    //  midPoint functions
    //  set reference point for both DC and AC
    void     setMidPoint(uint16_t midPoint);
    uint16_t getMidPoint();
    void     incMidPoint();
    void     decMidPoint();
    //  Auto midPoint, assuming zero DC current or any AC current
    void     autoMidPoint(float frequency = ACS712_DEFAULT_FREQ);


    //  Form Factor is also known as crest factor;
    //  affects mA_AC() only,  default sinus.
    void     setFormFactor(float formFactor = ACS712_FF_SINUS);
    float    getFormFactor();


    //  noise defaults 21 datasheet
    void     setNoisemV(uint8_t noisemV = ACS712_DEFAULT_NOISE);
    uint8_t  getNoisemV();


    //  Adjusting resolution AC and DC
    void     setmVperAmp(float mVperAmpere);
    float    getmVperAmp();
    float    getAmperePerStep();


    //  Frequency detection.
    //  the minimal frequency determines the time to sample.
    float    detectFrequency(float minimalFrequency = 40);
    void     setMicrosAdjust(float factor = 1.0);
    float    getMicrosAdjust();


  private:
    uint8_t   _pin;
    float     _mVperStep;
    float     _formFactor;    //  point2point -> RMS
    float     _mVperAmpere;
    float     _AmperePerStep;
    int       _midPoint;
    uint8_t   _noisemV;
    float     _microsAdjust = 1.0;  //  0.9986
};


// -- END OF FILE --

