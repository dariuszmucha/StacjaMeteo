#ifndef _CDUSTSENSOR_H_
#define _CDUSTSENSOR_H_

#include "CDustSensorRS232.h"

typedef enum DustSensorRequestID_t
{
  dataReportingModeRequestID = DUSTSENSOR_REPORTMODEID,
  pmUpdateRequestID = DUSTSENSOR_QUERYUPDATEID,
  setSleepWorkID = DUSTSENSOR_SETSLEEPWORKID,
  fwVersionRequestID = DUSTSENSOR_CHECKFWID
} DustSensorRequestID;

class CDustSensor
{
  private: 
    CDustSensorRS232 transport;
    uint16_t  pm25;
    uint16_t  pm10;
    uint32_t  fwVersion;
    uint8_t   workMode;
    uint8_t   reportMode;
    const uint8_t responseRetryCount = 100; // * 50ms

    uint8_t processFrame(DustSensorRespFrame *frame)
    {
      uint8_t processedId = DUSTSENSOR_NONEPROCESSEDID;
      debugDust("processFrame frame ID " + String(frame->cmd_id, HEX) + "\n\r");
      switch(frame->cmd_id)
      {
        case CMDID_PMSENSORUPDATE:
        {
          DustSensorPMRespFrame *pmData = (DustSensorPMRespFrame*)frame;
          pm25 = (uint16_t)pmData->pm25High << 8 | (uint16_t)pmData->pm25Low;
          pm10 = (uint16_t)pmData->pm10High << 8 | (uint16_t)pmData->pm10Low;
          debugDust("New PM10 = "); debugDust(pm10);
          debugDust("New PM25 = "); debugDust(pm25);
          debugDust("\n\r");
          processedId = DUSTSENSOR_QUERYUPDATEID;
        }
        break;

        case CMDID_SENSORRESP:
        {
          if(HandleCmdResponse((DustSensorCmdRespFrame*)frame))
          {
            processedId = frame->req_id;
          }
        }
        break;
      }
      return processedId;
    }

    bool HandleCmdResponse(DustSensorCmdRespFrame *frame)
    {
      bool processedSuccessfully = false;
      switch(frame->req_id)
      {
        case DUSTSENSOR_REPORTMODEID:
        {
          reportMode = frame->data[1];
          debugDust("Repord Mode active/query = ");
          debugDust(String(reportMode));
          debugDust("\n\r");
          processedSuccessfully = true;
        }
        break;
        
        case DUSTSENSOR_CHECKFWID:
        {
          fwVersion = ( (uint32_t)frame->data[0] << 24) | ( (uint32_t)frame->data[1] << 16) | (uint32_t)frame->data[2];
          debugDust("Fw version " + String(fwVersion, HEX) + "\n\r");
          processedSuccessfully = true;
        }
        break;

        case DUSTSENSOR_SETSLEEPWORKID:
        {
          workMode = frame->data[1];
          debugDust("Work Mode sleep/work = ");
          debugDust(String(workMode));
          debugDust("\n\r");
          processedSuccessfully = true;
        }
      }
      return processedSuccessfully;
    }

    uint8_t CalculateCRC(DustSensorCmdFrame *frame)
    {
      uint32_t sum = 0;
      for(uint8_t i = 0; i < 12; i++)
      {
        sum += frame->data_b[i];
      }
      sum += frame->request_id;
      sum += frame->deviceIdHi;
      sum += frame->deviceIdLo;
      return (uint8_t)(sum & 0xFF);
    }

    bool SendSleepWorkMode(uint8_t workMode)
    {
      if(workMode == DUSTSENSOR_SETSLEEPMODE || workMode == DUSTSENSOR_SETWORKMODE)
      {
        DustSensorCmdSetSleepWorkFrame frame;
        memset(&frame, 0, sizeof(DustSensorCmdSetSleepWorkFrame));

        frame.head = DUSTSENSOR_FRAMEHEAD;
        frame.cmd_id = CMDID_SENSORREQUEST;
        frame.request_id = DUSTSENSOR_SETSLEEPWORKID;
        frame.setOrQuery = DUSTSENSOR_SETNEWVAL;
        frame.workMode = workMode;
        frame.deviceIdHi = DUSTSENSOR_DEVID_ALL_LO;
        frame.deviceIdLo = DUSTSENSOR_DEVID_ALL_HI;
        frame.crc = CalculateCRC((DustSensorCmdFrame*)&frame);
        frame.tail = DUSTSENSOR_FRAMETAIL;
        
        return transport.SendFrame((DustSensorCmdFrame*)&frame);
      }
      return false;
    }

    bool SendDataReportingMode(uint8_t reportingMode)
    {
      if(reportingMode == DUSTSENSOR_REPORTACTIVEMODE || reportingMode == DUSTSENSOR_REPORTQUERYMODE)
      {
        DustSensorCmdReportingModeFrame frame;
        memset(&frame, 0, sizeof(DustSensorCmdReportingModeFrame));

        frame.head = DUSTSENSOR_FRAMEHEAD;
        frame.cmd_id = CMDID_SENSORREQUEST;
        frame.request_id = DUSTSENSOR_REPORTMODEID;
        frame.setOrQuery = DUSTSENSOR_SETNEWVAL;
        frame.reportingMode = reportingMode;
        frame.deviceIdHi = DUSTSENSOR_DEVID_ALL_LO;
        frame.deviceIdLo = DUSTSENSOR_DEVID_ALL_HI;
        frame.crc = CalculateCRC((DustSensorCmdFrame*)&frame);
        frame.tail = DUSTSENSOR_FRAMETAIL;
        
        return transport.SendFrame((DustSensorCmdFrame*)&frame);
      }
      return false;
    }

    bool SendFWVersion()
    {
      DustSensorCmdFWVerFrame frame;
      memset(&frame, 0, sizeof(DustSensorCmdFWVerFrame));

      frame.head = DUSTSENSOR_FRAMEHEAD;
      frame.cmd_id = CMDID_SENSORREQUEST;
      frame.request_id = DUSTSENSOR_CHECKFWID;
      frame.deviceIdHi = DUSTSENSOR_DEVID_ALL_LO;
      frame.deviceIdLo = DUSTSENSOR_DEVID_ALL_HI;
      frame.crc = CalculateCRC((DustSensorCmdFrame*)&frame);
      frame.tail = DUSTSENSOR_FRAMETAIL;
      
      return transport.SendFrame((DustSensorCmdFrame*)&frame);
    }

    bool RequestPMUpdate()
    {
      DustSensorCmdFrame frame;
      memset(&frame, 0, sizeof(DustSensorCmdFrame));

      frame.head = DUSTSENSOR_FRAMEHEAD;
      frame.cmd_id = CMDID_SENSORREQUEST;
      frame.request_id = DUSTSENSOR_QUERYUPDATEID;
      frame.deviceIdHi = DUSTSENSOR_DEVID_ALL_LO;
      frame.deviceIdLo = DUSTSENSOR_DEVID_ALL_HI;
      frame.crc = CalculateCRC((DustSensorCmdFrame*)&frame);
      frame.tail = DUSTSENSOR_FRAMETAIL;
      
      return transport.SendFrame((DustSensorCmdFrame*)&frame);  
      
    }

  public:
    CDustSensor()
    {
      pm25 = 0;
      pm10 = 0;
      fwVersion = 0;
      workMode = DUSTSENSOR_SETWORKMODE;
      reportMode = DUSTSENSOR_REPORTACTIVEMODE;
    }
  
    bool ReceivedResponse(DustSensorRequestID requestId)
    {
      DustSensorRespFrame_t frame;
      uint8_t processedFrames = 0;
      uint8_t retries = responseRetryCount;
      uint8_t test = 0;
      
      delay(50);
      
      while(retries-- > 0)
      {
        test++;
        if(transport.GetFrame(&frame))
        {
          uint8_t processedReqId = processFrame(&frame);
          if(requestId == processedReqId)
          {
            return true;
          }
        }
        delay(50);
      }
      return false;
    }

    bool Request(DustSensorRequestID requestId, uint8_t data)
    {
      bool sendResult = false;
      switch(requestId)
      {
        case dataReportingModeRequestID:
        {
          sendResult = SendDataReportingMode(data);
        }
        break;

        case setSleepWorkID:
        {
          sendResult = SendSleepWorkMode(data);
        }
        break;
      }
      if(sendResult)
      {
        return ReceivedResponse(requestId);
      }
      return false;
    }

    bool Request(DustSensorRequestID requestId)
    {
      bool sendResult = false;
      switch(requestId)
      {        
        case fwVersionRequestID:
        {
          sendResult = SendFWVersion();
        }
        break;

        case pmUpdateRequestID:
        {
          sendResult = RequestPMUpdate();
        }
        break;
      }
      if(sendResult)
      {
        return ReceivedResponse(requestId);
      }
      return false;
    }

  uint16_t GetPM25() { return pm25;};
  uint16_t GetPM10() { return pm10;};
};

#endif

