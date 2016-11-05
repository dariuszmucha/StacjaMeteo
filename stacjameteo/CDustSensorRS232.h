#ifndef _CDUSTSENSORRS232_H_
#define _CDUSTSENSORRS232_H_

#define DUSTSENSOR_FRAMEHEAD       0xAA
#define DUSTSENSOR_FRAMETAIL       0xAB
#define DUSTSENSOR_BUF_SIZE   10

#define CMDID_PMSENSORUPDATE  0xC0
#define CMDID_SENSORRESP      0xC5

#define CMDID_SENSORREQUEST   0xB4

#define DUSTSENSOR_QUERYMODE  0x00
#define DUSTSENSOR_SETNEWVAL  0x01

#define DUSTSENSOR_REPORTACTIVEMODE   0x00
#define DUSTSENSOR_REPORTQUERYMODE    0x01

#define DUSTSENSOR_SETSLEEPMODE       0x00
#define DUSTSENSOR_SETWORKMODE        0x01

#define DUSTSENSOR_NONEPROCESSEDID    0x00
#define DUSTSENSOR_REPORTMODEID       0x02
#define DUSTSENSOR_QUERYUPDATEID      0x04
#define DUSTSENSOR_SETSLEEPWORKID     0x06
#define DUSTSENSOR_CHECKFWID          0x07

#define DUSTSENSOR_DEVID_ALL_LO   0xFF
#define DUSTSENSOR_DEVID_ALL_HI   0xFF

#define PRINT_DEBUG_DUSTRS232x
#ifdef PRINT_DEBUG_DUSTRS232
#define debugDust(x) Serial.print(x)
#else
#define debugDust
#endif

typedef struct DustSensorRespFrame_t
{
  uint8_t cmd_id;
  uint8_t req_id;
  uint8_t data_b[5];
  uint8_t crc;
} DustSensorRespFrame;

typedef struct DustSensorPMRespFrame_t
{
  uint8_t cmd_id;
  uint8_t pm25Low;
  uint8_t pm25High;
  uint8_t pm10Low;
  uint8_t pm10High;
  uint8_t deviceId[2];
  uint8_t crc;
} DustSensorPMRespFrame;

typedef struct DustSensorSleepWorkRespFrame_t
{
  uint8_t cmd_id;
  uint8_t req_id;
  uint8_t queryOrSet;
  uint8_t sleepOrWork;
  uint8_t reserved;
  uint8_t deviceId[2];
  uint8_t crc;
} DustSensorSleepWorkRespFrame;

typedef struct DustSensorCmdRespFrame_t
{
  uint8_t cmd_id;
  uint8_t req_id;
  uint8_t data[3];
  uint8_t deviceId[2];
  uint8_t crc;
} DustSensorCmdRespFrame;

typedef struct DustSensorCmdFrame_t
{
  uint8_t head;
  uint8_t cmd_id;
  uint8_t request_id;
  uint8_t data_b[12];
  uint8_t deviceIdHi;
  uint8_t deviceIdLo;
  uint8_t crc;
  uint8_t tail;
} DustSensorCmdFrame;

typedef struct DustSensorCmdReportingModeFrame_t
{
  uint8_t head;
  uint8_t cmd_id;
  uint8_t request_id;
  uint8_t setOrQuery;
  uint8_t reportingMode;
  uint8_t reserved[10];
  uint8_t deviceIdHi;
  uint8_t deviceIdLo;
  uint8_t crc;
  uint8_t tail;
} DustSensorCmdReportingModeFrame;

typedef struct DustSensorCmdSetSleepWorkFrame_t
{
  uint8_t head;
  uint8_t cmd_id;
  uint8_t request_id;
  uint8_t setOrQuery;
  uint8_t workMode;
  uint8_t reserved[10];
  uint8_t deviceIdHi;
  uint8_t deviceIdLo;
  uint8_t crc;
  uint8_t tail;
} DustSensorCmdSetSleepWorkFrame;

typedef struct DustSensorCmdFWVerFrame_t
{
  uint8_t head;
  uint8_t cmd_id;
  uint8_t request_id;
  uint8_t reserved[12];
  uint8_t deviceIdHi;
  uint8_t deviceIdLo;
  uint8_t crc;
  uint8_t tail;
} DustSensorCmdFWVerFrame;

class CDustSensorRS232
{
  private:
    uint8_t buf[DUSTSENSOR_BUF_SIZE];
  public:
    CDustSensorRS232()
    {
      Uart Serial1( &sercom0, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX, PAD_SERIAL1_TX );   
      while (!Serial1);
      Serial1.begin(9600);
      debugDust("CDustSensorRS232\n\r");
    }

    bool SendFrame(DustSensorCmdFrame* frame)
    {
      debugDust("CDustSensorRS232 SendFrame\n\r");
      uint8_t bytesSent = Serial1.write((uint8_t*)frame, sizeof(DustSensorCmdFrame_t));
      debugDust("Bytes sent " + String(bytesSent) + "\n\r");
      return bytesSent == sizeof(DustSensorCmdFrame_t);
    }
  
    uint8_t GetFrame(DustSensorRespFrame* frame)
    {
      debugDust("CDustSensorRS232 GetFrame\n\r");
      
      uint8_t bytesToRead = Serial1.available();
      if(bytesToRead > DUSTSENSOR_BUF_SIZE) bytesToRead = DUSTSENSOR_BUF_SIZE;

      debugDust("Bytes to read = ");
      debugDust(String(bytesToRead));
      debugDust("\n\r");
      
      memset(this->buf, 0, DUSTSENSOR_BUF_SIZE);
      
      Serial1.readBytes(this->buf, bytesToRead);
  
      uint8_t *ptr = &(this->buf[0]);

      debugDust("Received = ");
      for(uint8_t ptrPos = 0; ptrPos < DUSTSENSOR_BUF_SIZE; ptrPos++)
      {
        debugDust(String(ptr[ptrPos]));
        debugDust("|");
      }
      
      debugDust("\n\r");
      
      uint8_t i = 0;
      while(ptr[i] != DUSTSENSOR_FRAMEHEAD && ptr[i+9] != DUSTSENSOR_FRAMETAIL)
      {
        i++;
        if(i+9 >= DUSTSENSOR_BUF_SIZE) break;
      }
  
      if(i+9 >= DUSTSENSOR_BUF_SIZE)
      {
        debugDust("Error, reached buf limit\n\r");
        return 0;
      }
      else
      {
        memcpy(frame, &ptr[i+1], sizeof(DustSensorRespFrame_t));
        debugDust("OK\n\r");
        return 1;
      }
    }

    
};

#endif

