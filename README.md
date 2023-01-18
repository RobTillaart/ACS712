
[![Arduino CI](https://github.com/RobTillaart/ACS712/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/ACS712/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/ACS712/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/ACS712/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/ACS712/actions/workflows/jsoncheck.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/ACS712/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/ACS712.svg?maxAge=3600)](https://github.com/RobTillaart/ACS712/releases)


# ACS712

Library for the ACS712 Current Sensor - 5A, 20A, 30A and compatibles.


## Description

The ACS712 is a chip to measure current, both AC or DC. The chip has an
analogue output that provides a voltage that is linear with the current.
The ACS712 library supports only a built in ADC by means of **analogRead()**.
There are 4 core functions:

- **float mA_peak2peak(frequency = 50, cycles = 1)**
- **float mA_DC(cycles = 1)**
- **float mA_AC(frequency = 50, cycles = 1)**
- **float mA_AC_sampling(frequency = 50, cycles = 1)** 

The parameter cycles is used to do measure multiple cycles and average them.

To measure DC current a single **analogRead()** with conversion math is 
sufficient to get a value. 
To stabilize the signal **analogRead()** is called at least twice.

To measure AC current **a blocking loop for 20 milliseconds** (50 Hz, 1 cycle) 
is run to determine the peak to peak value which is converted to the RMS value. 
To convert the peak2peak value to RMS one need the so called crest or form factor. 
This factor depends heavily on the signal form, hence its name.
For a perfect sinus the value is sqrt(2)/2 == 1/sqrt(2).
See **Form factor** below.

The **mA_AC_sampling()** calculates the average of the sumSquared of many measurements.
It should be used when the form factor is not known.

Note to make precise measurements, the power supply of both the ACS712 and the ADC of
the processor should be as stable as possible. 
That improves the stability of the midpoint and minimizes the noise.


#### Resolution

|  Sensor  |  mVperA  |  LSB 10bit  |  LSB 12bit  |  LSB 16bit  |
|:---------|:--------:|:-----------:|:-----------:|:-----------:|
|    5 A   |    185   |   26.4 mA   |    6.6 mA   |   0.41 mA   |
|   20 A   |    100   |   48.9 mA   |   12.2 mA   |   0.76 mA   |
|   30 A   |     66   |   74.1 mA   |   18.5 mA   |   1.16 mA   |

```cpp
getmAPerStep();
mA LSB = (5000 mV / maxADC) / mVperA * 1000.0;
mA LSB = (1000 * 5000 mV) / (maxADC * mVperA);
```

Although no 16 bit ADC built in are known, it indicates what resolution 
could be obtained with such an ADC. It triggered the thought for supporting
external ADC's with this library or a derived version. See future.


#### Tests

The library is at least confirmed to work with the following boards:

| Device       | Voltage | ADC steps |  Notes  |
|:-------------|:-------:|:---------:|:--------|
| Arduino UNO  |  5.0V   |   1024    | tested with RobotDyn ACS712 20 A breakout.
| Arduino UNO  |  5.0V   |   1024    | tested with Open-Smart ACS712 5 A breakout.
| Arduino NANO |  5.0V   |   1024    | #18
| ESP32        |  3.3V   |   4096    | #15
| Promicro     |  5.0V   |   1024    | #15 

Please let me know of other working platforms / processors.



## Compatibles

Robodyn has a breakout for the ACS758 - 50 A. - See resolution below.
This sensor has versions up to 200 Amps, so use with care!

Allegromicro offers a lot of different current sensors, that might be compatible.
These include bidirectional and unidirectional. 
The unidirectional seem to be for DC only.

https://www.allegromicro.com/en/products/sense/current-sensor-ics/current-sensors-innovations

If you have tested a compatible sensor, please share your experiences.
(can be done by opening an issue to update documentation)


#### Resolution ACS758

Not tested, but looks compatible - same formula as above

|  Sensor  |  mVperA  |  LSB 10bit  |  LSB 12bit  |  LSB 16bit  |  directional  |
|:---------|:--------:|:-----------:|:-----------:|:-----------:|:-------------:|
|   50 A   |     40   |  122.2 mA   |   30.5 mA   |   1.91 mA   |     bi        |
|   50 A   |     60   |   81.5 mA   |   20.3 mA   |   1.27 mA   |     uni       |
|  100 A   |     20   |  244.4 mA   |   61.0 mA   |   3.81 mA   |     bi        |
|  100 A   |     40   |  122.2 mA   |   30.5 mA   |   1.91 mA   |     uni       |
|  150 A   |   13.3   |  367.5 mA   |   91.8 mA   |   5.74 mA   |     bi        |
|  150 A   |   26.7   |  183.1 mA   |   45.7 mA   |   2.86 mA   |     uni       |
|  200 A   |     10   |  488.8 mA   |  122.1 mA   |   7.63 mA   |     bi        |
|  200 A   |     20   |  244.4 mA   |   61.0 mA   |   3.81 mA   |     uni       |


## Interface

#### Base

- **ACS712(uint8_t analogPin, float volts = 5.0, uint16_t maxADC = 1023, float mVperAmpere = 100)** constructor. 
It defaults a 20 A type sensor, which is defined by the default value of mVperAmpere. See table below.
Volts is the voltage used by the (Arduino) internal ADC. maxADC is the maximum output of the internal ADC.
The defaults are based upon an Arduino UNO, 10 bits ADC.
These two ADC parameters are needed to calculate the voltage output of the ACS712 sensor.
- **float mA_peak2peak(float frequency = 50, uint16_t cycles = 1)** blocks ~21 ms to sample a whole 50 or 60 Hz period.
Returns the peak to peak current, can be used to determine form factor.  
The **mA_peak2peak()** can also be used to measure on a zero current line 
to get an indication of the lowest detectable current.
Finally this function is used internally to detect the noiseLevel in mV on a zero current line. 
- **float mA_AC(float frequency = 50, uint16_t cycles = 1)** blocks ~21 ms to sample a whole 50 or 60 Hz period.
Note that a lower frequency, or more cycles, will increase the blocking period.
The function returns the AC current in mA.
Its working is based upon multiplying the peak2peak value by the FormFactor which must be known and set.
  - 0.2.2 frequencies other integer values than 50 and 60 are supported.
  - 0.2.3 floating point frequencies are supported to tune even better.
  - 0.2.8 the parameter cycles allow to average over a number of cycles.
- **float mA_AC_sampling(float frequency = 50, uint16_t cycles = 1)** blocks ~21 ms to sample a whole period.
The function returns the AC current in mA. (Note it returns a float).
Its working is based upon sampling a full period and take the square root of the average sumSquared.
This function is intended for signals with unknown Form Factor.
  - 0.2.8 the parameter cycles allow to average over a number of cycles.
- **float mA_DC(uint16_t samples = 1)** blocks < 1 ms (Arduino UNO) as it calls **analogRead()** twice.
A negative value indicates the current flows in the opposite direction.
  - 0.2.8 the parameter samples allow to average over a number of samples.


#### Midpoint

The midpoint is the (raw) zero-reference for all current measurements.
It is defined in steps of the ADC and is typical around half the **maxADC** value defined 
in the constructor. So for a 10 bit ADC a number between 500..525 is most likely.

Since 0.3.0 all midpoint functions return the actual midPoint.

- **uint16_t setMidPoint(uint16_t midPoint)** sets midpoint for the ADC conversion.
Parameter must be between 0 and maxADC/2, otherwise midpoint is not changed.
- **uint16_t autoMidPoint(float frequency = 50, uint16_t cycles = 1)** Auto midPoint, 
assuming zero DC current or any AC current. 
The function takes the average of many measurements during one or more full cycles.
Note the function therefore blocks for at least 2 periods. 
By increasing the number of cycles the function averages even more measurements, 
possibly resulting in a better midPoint. Idea is that noise will average out. 
This function is mandatory for measuring AC.
  - 0.2.2 frequencies other than 50 and 60 are supported.
  - 0.2.8 the parameter cycles allow to average over a number of cycles.
- **uint16_t getMidPoint()** read the value set / determined.
- **uint16_t incMidPoint()** manual increase midpoint, e.g. useful in an interactive application.
Will not increase if midpoint equals macADC.
- **uint16_t decMidPoint()** manual decrease midpoint.
Will not decrease if midpoint equals 0.
- **uint16_t resetMidPoint()** resets the midpoint to the initial value of maxADC / 2 as in the constructor.

Since version 0.3.0 there is another way to determine the midPoint.
One can use the two debug functions. 
(milliseconds > 20 to get at least a full cycle)
- **uint16_t getMinimum(uint16_t milliSeconds = 20)**
- **uint16_t getMaximum(uint16_t milliSeconds = 20)**

and take the average of these two values. In code:

```cpp
uint16_t midpoint = ACS.setMidPoint(ACS.getMinimum(20)/2 + ACS.getMaximum(20)/ 2);
```
See - ACS712_20_AC_midPoint_compare.ino

The ACS712 has a midPoint level that is specified as  0.5 \* VCC. 
So **autoMidPoint()** can help to detect voltage deviations for the ACS712. 
The library does not support this yet.


#### Form factor 

The form factor is also known as the crest factor. 
It is only used for signals measured with **mA_AC()**.

- **void setFormFactor(float formFactor = ACS712_FF_SINUS)** manually sets the form factor.
Must typical be between 0.0 and 1.0, see constants below.
- **float getFormFactor()** returns current form factor. 

The library has a number of predefined form factors:

|  definition          | value         | approx |  notes  |
|:---------------------|:--------------|:------:|:--------|
| ACS712_FF_SQUARE     | 1.0           | 1.000  |         |
| ACS712_FF_SINUS      | 1.0 / sqrt(2) | 0.707  | default |
| ACS712_FF_TRIANGLE   | 1.0 / sqrt(3) | 0.577  |         |
| ACS712_FF_SAWTOOTH   | 1.0 / sqrt(3) | 0.577  |         |

It is important to measure the current with a calibrated multimeter
and determine / verify the form factor of the signal. 
This can help to improve the quality of your measurements.

Please let me know if other crest factors need to be added.

Since version 0.3.0 the form factor can be determined by

```cpp
float formFactor = 2.0 * mA_AC_sampling() / ACS.mA_peak2peak();
```

See - ACS712_20_determine_form_factor.ino


#### Noise

Default = 21 mV (datasheet)

- **void setNoisemV(uint8_t noisemV = 21)** sets the noise level, 
is used to determine zero level e.g. in the AC measurements with **mA_AC()**.
- **uint8_t getNoisemV()** returns the set value.
- **float mVNoiseLevel(float frequency, uint16_t cycles)** determines the mV of noise.
Measurement should be taken when there is no AC/DC current or a constant DC current. 
The level will give a (not quantified yet) indication of the accuracy of the measurements.
A first order indication can be made by comparing it to voltage / 2 of the constructor.

Noise on the signal can be reduced by using a low pass (RC) filter. 
Version 0.3.1 includes experimental code to take two sample and average them.
The idea is that ```((3 + 5)/2)^2   <  (3^2 + 5^2)/2```

In theory this should suppress noise levels however more investigation in
software noise detection and suppression is needed.

- **void suppressNoise(bool flag)** experimental noise suppression.


#### mV per Ampere

Used for both for AC and DC measurements. 
Its value is defined in the constructor and depends on type sensor used.
These functions allow to adjust this setting run-time.

- **void setmVperAmp(float mVperAmpere)** sets the milliVolt per Ampere measured.
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


#### setADC (experimental 0.3.4)

- **void setADC(uint16_t (\*)(uint8_t), float volts, uint16_t maxADC)** sets the ADC function and the parameters of the used ADC.
The library uses the internal **analogRead()** as default by means of this
wrapper in ACS712.h:

```cpp
static uint16_t _internalAnalog(uint8_t pin)
{
  return analogRead(pin);
}
```

Be sure to set the parameters of the constructor correctly.

- example ACS712_20_DC_external_ADC.ino
- https://github.com/RobTillaart/ACS712/issues/31

Note that the use of an external ADC should meet certain performance requirements, 
especially for measuring **ma-AC()**.
To 'catch' the peaks well enough one needs at least 2 samples per millisecond

The 16 bit I2C **ADS1115** in continuous mode gives max 0.8 samples per millisecond. 
This will work perfect for high resolution **mA-DC()** but is not fast enough for
doing **mA-AC()**.

The SPI based **MCP3202** ao can do up to 100 samples per millisecond at 12 bit. 
These ADC's are perfect both **mA-DC()** and **mA-AC()**.

- https://github.com/RobTillaart/ADS1X15
- https://github.com/RobTillaart/MCP_ADC


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
including the "offset" from the **midPoint** zero current level.

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


**Note:** setting the midPoint correctly is also needed when using a voltage divider.


## Disconnect detection

(to be tested)

To detect that the ACS712 is disconnected from the ADC one could connect the 
analog pin via a pull-down to GND. A pull-up to VCC is also possible.
Choose the solution that fits your project best. (Think safety).

**mA_DC()** and **mA_AC_sampling()** will report HIGH values (Out of range) when 
the ACS712 is disconnected.
The other - peak2peak based functions - will see this as zero current (min == max).

Schema with PULL-UP.
```
                 ACS712 OUT
                     |
                     |
   VCC ----[ R1 ]----o       R1 = 1 M ohm.
                     |
                     |
                ADC of processor
```

The library does not support this "extreme values" detection.


## Operation

The examples show the basic working of the functions.


## Future

#### Must


#### Should - 0.3.x

- investigate noise suppression  #21 (0.3.1 and later)
- setADC() should be able to reset to internal.


#### Could

- merge **mA_AC()** and **mA_AC_sampling()** into one. (0.4.0)
  - or remove - depreciate - the worst one
- investigate blocking calls:
  - **mA_AC()** blocks for about 20 ms at 50 Hz.
  This might affect task scheduling on a ESP32. Needs to be investigated. 
  Probably need a separate thread that wakes up when new analogRead is available?
  - RTOS specific class?
  - **detectFrequency(float)** blocks pretty long.
- other set functions also a range check?
- split the readme.md in multiple documents?
  - which?
- setADC() to support > 16 bit?
  - uint32_t performance penalty?


#### Won't

- investigate support for micro-Amperes. **ACS.uA_DC()**
  - need a very stable voltage 
  - needs a 24 bit ADC 
  - default noise is already ~21mV...
  - => not feasible in normal setup.
- Should the FormFactor not be just a parameter of **mA_AC()**
  it is the only function using it. ==> No unnecessary breaking API
- should cycles be an uint8_t ?
  - No, uint16 allows averaging in minutes range uint8_t just ~5 seconds
- midPoint can be a float so it can be set more exact.
  - extra precision is max half bit = smaller than noise?
  - math will be slower during sampling (UNO)

