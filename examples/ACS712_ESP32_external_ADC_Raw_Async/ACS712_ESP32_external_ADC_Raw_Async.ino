//    FILE: ACS712_ESP32_external_ADC_Raw_Async.ino
//  AUTHOR: Stian Blåsberg
// PURPOSE: demo to measure mA DC with external ADC using raw values - asynchronous
//     URL: https://github.com/RobTillaart/ACS712

#include "Arduino.h"
#include "ACS712.h"
#include "ADS1X15.h"


//  I2C config
#define ADS1015_ADDRESS         0x48
#define ADS1015_SCL             22   //  default SCL ESP32
#define ADS1015_SDA             21   //  default SDA ESP32

//  ADS1x15 config
#define SENSOR_ACS712_ADSPIN    1

uint16_t rawAdsValue;


//  explicit parameters for demo
ADS1015 ads1015(ADS1015_ADDRESS, &Wire);  //  ADS1015 == 12 bit


//  SENSOR_ACS712_ADSPIN sets pin 1 of the ADS1015, 3.3 volt, 4095 = 12 bit, 100 = mVperAmpere
ACS712 ACS(SENSOR_ACS712_ADSPIN, 3.3, 4095, 100);

///////////////////////////////////////////////////////////////

void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println(__FILE__);
  Serial.print("ACS712_LIB_VERSION: ");
  Serial.println(ACS712_LIB_VERSION);
  Serial.print("ADS1X15_LIB_VERSION: ");
  Serial.println(ADS1X15_LIB_VERSION);
  Serial.println();


  //  ESP32 set wire pins explicitly
  Wire.begin(ADS1015_SDA, ADS1015_SCL);
  Wire.setClock(400000);


  //  initialize ADS1015, if fail => report
  if (ads1015.begin() == false)
  {
    Serial.println("ADS1x15 not found. Check wires and pins. Reboot.");
    while(1);
  }

  //  set up the external ADC for the ACS712
  ACS.setADCRaw(rawAdsValue, 3.3, 4095);
  ads1015.requestADC(0);
}


void loop()
{
  if (ads1015.isBusy() == false)
  {
    int16_t val_0 = ads1015.getValue();
    //  request a new one
    ads1015.requestADC(0);
    int mA = ACS.mA_DC();
    Serial.println(mA);
  }
  //  simulate other tasks...
  delay(1000);
}


//  -- END OF FILE --
