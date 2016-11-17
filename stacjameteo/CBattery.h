#ifndef _CBATTERY_H_
#define _CBATTERY_H_

#include <Arduino.h>

#define VBATFULLADC   651 // *2 * 3.3 / 1024 [mV] 
#define VBATEMPTYADC  589 // *2 * 3.3 / 1024 [mV]
class CBattery
{
  private:
    const uint16_t pin;
    uint8_t battLevel;
    
    uint8_t calculateBattLevel()
    {
      uint16_t measuredvbatadc = analogRead(pin);
      
      if(measuredvbatadc > VBATFULLADC)
      {
        measuredvbatadc = VBATFULLADC;
      }
      else if(measuredvbatadc < VBATEMPTYADC)
      {
        measuredvbatadc = VBATEMPTYADC;
      }

      measuredvbatadc = (measuredvbatadc - VBATEMPTYADC) * 100;
      return measuredvbatadc / (VBATFULLADC - VBATEMPTYADC);
    }
    
  public:
    CBattery(uint16_t adcPin) : pin(adcPin)
    { }

    uint8_t GetBattLevel()
    {
      battLevel = calculateBattLevel();
      return battLevel;
    }
};

#endif

