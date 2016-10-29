#ifndef _CDUSTSENSOR_H_
#define _CDUSTSENSOR_H_

#include "CDustSensorRS232.h"



class CDustSensor
{
  private: 
    CDustSensorRS232 transport;
    uint16_t pm25;
    uint16_t pm10;

    uint8_t processFrame(DustSensorRespFrame_t *frame)
    {
      switch(frame->cmd_id)
      {
        case CMDID_PMSENSORUPDATE:
          DustSensorPMRespFrame *pmData = (DustSensorPMRespFrame*)frame;
          pm25 = (uint16_t)pmData->pm25High << 8 | (uint16_t)pmData->pm25Low;
          pm10 = (uint16_t)pmData->pm10High << 8 | (uint16_t)pmData->pm10Low;
          debug("New PM10 = "); debug(pm10);
          debug("New PM25 = "); debug(pm25);
          debug("\n\r");
        break;
      }
    }

  public:
    CDustSensor()
    {
      
    }
  
    uint8_t Update()
    {
      DustSensorRespFrame_t frame;
      while(transport.GetFrame(&frame))
      {
        processFrame(&frame);
      }
    }

  uint16_t GetPM25() { return pm25;};
  uint16_t GetPM10() { return pm10;};
};

#endif

