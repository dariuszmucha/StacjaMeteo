#ifndef _CDUSTSENSORRS232_H_
#define _CDUSTSENSORRS232_H_

#define DUSTSENSOR_HEAD       0xAA
#define DUSTSENSOR_TAIL       0xAB
#define DUSTSENSOR_BUF_SIZE   20

#define CMDID_PMSENSORUPDATE  0xC0

#ifdef PRINT_DEBUG_DUSTRS232
#define debug(x) Serial.println(x)
#else
#define debug
#endif

typedef struct DustSensorRespFrame_t
{
  uint8_t cmd_id;
  uint8_t data_b_1;
  uint8_t data_b_2;
  uint8_t data_b_3;
  uint8_t data_b_4;
  uint8_t data_b_5;
  uint8_t data_b_6;
  uint8_t crc;
} DustSensorRespFrame;

typedef struct DustSensorPMRespFrame_t
{
  uint8_t cmd_id;
  uint8_t pm25Low;
  uint8_t pm25High;
  uint8_t pm10Low;
  uint8_t pm10High;
  uint8_t reserved0;
  uint8_t reserved1;
  uint8_t crc;
} DustSensorPMRespFrame;

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
      debug("CDustSensorRS232\n\r");
    }
  
    uint8_t GetFrame(DustSensorRespFrame_t* frame)
    {
      debug("CDustSensorRS232 GetFrame\n\r");
      
      uint8_t bytesToRead = Serial1.available();
      if(bytesToRead > DUSTSENSOR_BUF_SIZE) bytesToRead = DUSTSENSOR_BUF_SIZE;

      debug("Bytes to read = ");
      debug(String(bytesToRead));
      debug("\n\r");
      
      memset(this->buf, 0, DUSTSENSOR_BUF_SIZE);
      
      Serial1.readBytes(this->buf, bytesToRead);
  
      uint8_t *ptr = &(this->buf[0]);

      debug("Received = ");
      for(uint8_t ptrPos = 0; ptrPos < DUSTSENSOR_BUF_SIZE; ptrPos++)
      {
        debug(String(ptr[ptrPos]));
        debug("|");
      }
      
      debug("\n\r");
      
      uint8_t i = 0;
      while(ptr[i] != DUSTSENSOR_HEAD && ptr[i+9] != DUSTSENSOR_TAIL)
      {
        i++;
        if(i+9 >= DUSTSENSOR_BUF_SIZE) break;
      }
  
      if(i+9 >= DUSTSENSOR_BUF_SIZE)
      {
        debug("Error, reached buf limit\n\r");
        return 0;
      }
      else
      {
        memcpy(frame, &ptr[i+1], sizeof(DustSensorRespFrame_t));
        debug("OK\n\r");
        return 1;
      }
    }

    
};

#endif

