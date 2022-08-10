
[![Arduino CI](https://github.com/RobTillaart/ACS712/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/ACS712/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/ACS712/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/ACS712/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/ACS712/actions/workflows/jsoncheck.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/ACS712/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/ACS712.svg?maxAge=3600)](https://github.com/RobTillaart/ACS712/releases)


# ACS712

Library for the ACS712 Current Sensor - 5A, 20A, 30A.


## Description

The ACS712 is a chip to measure current, both AC or DC. The chip has an
analogue output that provides a voltage that is linear with the current.
The ACS712 library supports only a built in ADC by means of **analogRead()**.
There are 2 core functions:

- **int mA_DC()**
- **int mA_AC(float freq = 50)** The frequency can be set to typically 50 or 60 Hz
however other values e.g. 50.1 or 40 or 123.456 are possible.

To measure DC current a single **analogRead()** with conversion maths is sufficient to get
a value. To stabilize the signal **analogRead()** is called twice.

To measure AC current **a blocking loop for 20 milliseconds** (50 Hz assumed) is run 
to determine the peak to peak value which is converted to the RMS value. 
To convert the peak2peak value to RMS one need the so called crest or form factor. 
This factor depends heavily on the signal form. 
For a perfect sinus the value is sqrt(2)/2 == 1/sqrt(2).
See Form factor below.


#### Tests

The library is at least confirmed to work with:

| Device      | Voltage | ADC steps |  Notes  |
|:------------|:-------:|:---------:|:--------|
| Arduino UNO |  5.0V   |   1024    | tested with RobotDyn ACS712 20 A breakout
| ESP32       |  3.3V   |   4096    | #15
| Promicro    |  5.0V   |   1024    | #15 

Please let me know of other working platforms.


## Interface

#### Base

- **ACS712(uint8_t analogPin, float volts = 5.0, uint16_t maxADC = 1023, float mVperA = 100)** constructor. 
It defaults a 20 A type sensor, which is defined by the default value of mVperA. See table below.
Volts is the voltage used by the (Arduino) internal ADC. maxADC is the maximum output of the internal ADC.
The defaults are based upon an Arduino UNO.
These two ADC parameters are needed to calculate the voltage output of the ACS712 sensor.
- **int mA_AC(float freq = 50)** blocks ~21 ms (depending on the freq) to sample a whole 50 or 60 Hz period.  
Since version 0.2.2 frequencies other integer values than 50 and 60 are supported, the lower the frequency, 
the longer the blocking period.
Since version 0.2.3 floating point frequencies are supported to tune even better.
- **int mA_DC()** blocks < 1 ms (Arduino UNO) as it calls **analogRead()** twice.
A negative value indicates the current flows in the other direction.


#### Resolution

|  Sensor  |  mVperA  |  LSB 10bit  |  LSB 12bit  |  LSB 16bit  |
|:---------|:--------:|:-----------:|:-----------:|:-----------:|
|  5 A     |  185     |   ~25 mA    |  ~06.25 mA  |  ~0.38 mA   |
|  20 A    |  100     |  ~100 mA    |  ~25.00 mA  |  ~1.54 mA   |
|  30 A    |  66      |  ~150 mA    |  ~37.50 mA  |  ~2.31 mA   |


#### Midpoint

- **void setMidPoint(uint16_t mp)** sets midpoint for the ADC conversion.
- **void autoMidPoint(float freq = 50)** Auto midPoint, assuming zero DC current or any AC current. 
Note it will block for 2 periods. Since version 0.2.2 frequencies other than 50 and 60 are supported.
By setting the frequency to e.g 1, the code will sample for 2 seconds, possibly getting a better average.
- **uint16_t getMidPoint()** read the value set / determined.
- **void incMidPoint()** manual increase midpoint, e.g. useful to manually adjust the midPoint in an interactive application.
- **void decMidPoint()** manual decrease midpoint.


#### Form factor 

Also known as crest factor;  affects AC signals only. 

- **void setFormFactor(float ff = ACS712_FF_SINUS)** manually sets form factor.
Must typical be between 0.0 and 1.0, see constants below.
- **float getFormFactor()** returns current form factor. 

The library has a number of predefined form factors:

|  definition          | value         | approx | notes   |
|:---------------------|:--------------|:------:|:--------|
| ACS712_FF_SQUARE     | 1.0           | 1.000  |         |
| ACS712_FF_SINUS      | 1.0 / sqrt(2) | 0.707  | default |
| ACS712_FF_TRIANGLE   | 1.0 / sqrt(3) | 0.577  |         |
| ACS712_FF_SAWTOOTH   | 1.0 / sqrt(3) | 0.577  |         |

It is important to measure the current with a calibrated multimeter
and determine / verify the form factor of the signal. 
This can help to improve the quality of your measurements.

Please let me know if other crest factors need to be added.


#### Noise

Default = 21 mV.

- **void setNoisemV(uint8_t noisemV = 21)** set noise level, 
is used to determine zero level e.g. in AC measurements.
- **uint8_t getNoisemV()** returns the set value.


#### mV per Ampere

Used for both for AC and DC measurements. 
Its value is defined in the constructor and depends on type sensor used.
These functions allow to adjust this setting run-time.

- **void setmVperAmp(float mva)** sets the milliVolt per Ampere measured.
- **float getmVperAmp()** returns the set value.

Typical values see "Resolution" section above, and the "voltage divider" section below.


#### Frequency detection

Experimental functionality for AC signal only!

- **float detectFrequency(float minimalFrequency = 40)** Detect the frequency of the AC signal.
- **void setMicrosAdjust(float factor = 1.0)** adjusts the timing of micros in **detectFrequency()**.
Values are typical around 1.0 ± 1%
- **float getMicrosAdjust()** returns the set factor. 

The minimum frequency of 40 Hz is used to sample for enough time to find the minimum and maximum
for 50 and 60 Hz signals. 
Thereafter the signal is sampled 10 cycles to minimize the variation of the frequency.

The **microsAdjust()** is to adjust the timing of **micros()**. 
This function is only useful if one has a good reference source like a calibrated function generator 
to find the factor to adjust. 
Testing with my UNO I got a factor 0.9986.

Current version is experimental and not performance optimized. 


## Voltage divider

As per issue #15 in which an ACS712 was connected via a voltage divider to the ADC of an ESP32.

Schema
```
ACS712 ----[ R1 ]----o----[ R2 ]---- GND
                     |
                     |
                ADC of processor
```

The voltage divider gave an error of about a factor 2 as all voltages were divided, 
including the "offset" from the zero current level.

By adjusting the mV per Ampere with **setmVperAmp(float mva)** the readings can be corrected 
for this "voltage divider effect".


#### Examples:

For a 20 A type sensor, 100 mV/A would be the normal value. 
After using a voltage divider one need to adjust the mVperAmp.

| R1 (ACS) | R2 (GND)  | voltage factor                  |  mVperAmp corrected     |
|:--------:|:---------:|:-------------------------------:|:-----------------------:|
|  10200   |  4745     |  4745 / (10200 + 4745) = 0.3175 |  100 \* 0.3175 = 31.75  |
|  4745    |  10200    | 10200 / (10200 + 4745) = 0.6825 |  100 \* 0.6825 = 68.25  |
|  10200   |  9800     |  9800 / (10200 + 9800) = 0.4900 |  100 \* 0.4900 = 49.00  |


**Note:** setting the midPoint correctly is needed when using a voltage divider.


## Operation

With the constructor the parameters **volts** and **maxADC (== steps-1)** of the ADC are set
together with the **milliVolt per Ampere** value. 
The last parameter can be adjusted afterwards, e.g. to calibrate this value runtime. 
Note this parameter affects both AC and DC measurements.

To calibrate the zero level for DC measurements, 5 functions are available to
adjust the midPoint.

To calibrate the RMS value for AC measurements, 2 functions are available to
get and set the formFactor.

To calibrate the noise level (used for AC measurements), 2 functions are available to
get and set the noise in mV.

The examples show the basic working of the functions.


## Future

#### Should

- investigate blocking calls:
  - **mA_AC()** blocks for about 20 ms at 50 Hz.
  This might affect task scheduling on a ESP32. Needs to be investigated. 
  Probably need a separate thread that wakes up when new analogRead is available?
  - **detectFrequency(float)** also blocks pretty long.
- investigate support for micro-Amperes. **ACS.uA_DC()**


#### Could

- do we need to add **int point2point(float freq)** function for AC. 
Is technically a part of mA_AC() already.  
Needs extra global variables, which are slower than local ones  
Or just cache the last p2p value?
- **midPOint()** functions could all return actual midPoint iso void.


#### Won't

- external analogue read support? separate class!

