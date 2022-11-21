# Change Log AD520X

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).


## [0.3.2] - 2022-11-18
- fix #26 revert data type \_midPoint to int
- Add CHANGELOG.md

## [0.3.1  2022-09-xx  
- add float mVNoiseLevel(frequency, cycles)
- add void suppressNoise(bool flag) - experimental suppression by averaging two samples.
- update readme.md
- improve midPoint functions
- add resetMidPoint()
- add RP2040 pico in build-ci

## [0.3.0] - 2022-09-01
- return midPoint value in MP functions.
- float return type for mA() functions
- add float mA_peak2peak(freq, cycles)
- add debug getMinimum(), getmaximum();
- update Readme.md

----

## [0.2.8] - 2022-08-19  prepare for 0.3.0
- Fix #21 FormFactor
- add mA_AC_sampling() as method to determine
- current when FormFactor is unknown.
- added float _AmperePerStep cached value.
- added getAmperePerStep();
- moved several functions to .cpp
- improve documentation

## [0.2.7] - 2022-08-10
- change mVperAmp to float
- add ACS712_FF_SAWTOOTH
- update readme.md + unit test + minor edits

## [0.2.6] - 2021-12-09
- update readme.md
- update license

## [0.2.5] - 2021-12-03
- add timeout to detectFrequency()

## [0.2.4] - 2021-11-22
- add experimental detectFrequency()

## [0.2.3] - 2021-10-15
- change frequencies to float, for optimal tuning.
- update build CI
- update readme.md

## [0.2.2] - 2021-06-23
- support for more frequencies

## [0.2.1] - 2020-12-06
- Add Arduino-CI + unit test
- update readme 
- refactor

## [0.2.0] - 2020-08-02
- Add autoMidPoint()

----

## [0.1.4] - 2020-08-02
- Allow for faster processors

## [0.1.3] - 2020-05-27
- fix library.json

## [0.1.2] - 2020-03-21
- automatic form factor test

## [0.1.1] - 2020-03-18
- first release version

## [0.1.0] - 2020-03-17
- initial version



