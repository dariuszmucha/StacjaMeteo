#ifndef _BLUEFRUITLECUSTOM_H_
#define _BLUEFRUITLECUSTOM_H_

#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

class CBluefruitLECustom : public Adafruit_BluefruitLE_SPI
{
private:

public:
  CBluefruitLECustom();
  
  void setAdvInterval();  
};

#endif

