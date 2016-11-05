#include <Arduino.h>
#include <SPI.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "DHT.h"

#include "BluefruitConfig.h"
#include "BluefruitLECustom.h"

#include "CDustSensor.h"

// Create the bluefruit object, either software serial...uncomment these lines
/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
CBluefruitLECustom ble = CBluefruitLECustom();

#define DHTPIN 14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
CDustSensor *dustSensor;

#define PRINT_DEBUG
#ifdef PRINT_DEBUG
#define debug(x) Serial.println(x)
#else
#define debug
#endif



class CSensorResults
{
  private:
    uint16_t* pm25Results;
    uint16_t* pm10Results;
    uint8_t* temperatureResults;
    uint8_t* humidityResults;

    uint8_t pos;
    const uint8_t arraySize;

    uint16_t CalcAvg(uint8_t *arr)
    {
      uint8_t calculatedElements = 0;
      uint32_t sum = 0;
      for(uint8_t i = 0; i < pos; i++)
      {
        if(arr[i] > 0)
        {
          sum += arr[i];
          calculatedElements++;
        }
      }
      return (uint16_t)(sum / calculatedElements);
    }

    uint16_t CalcAvg(uint16_t *arr)
    {
      uint8_t calculatedElements = 0;
      uint32_t sum = 0;
      for(uint8_t i = 0; i < pos; i++)
      {
        if(arr[i] > 0)
        {
          sum += arr[i];
          calculatedElements++;
        }
      }
      return (uint16_t)(sum / calculatedElements);
    }
    
  public:
    CSensorResults(uint8_t aSize) : arraySize(aSize) 
    { 
      pm25Results = new uint16_t[arraySize];
      pm10Results = new uint16_t[arraySize];
      temperatureResults = new uint8_t[arraySize];
      humidityResults = new uint8_t[arraySize];
      Clear();
      pos = 0;
    } 
    ~CSensorResults()
    { 
      delete[] pm25Results;
      delete[] pm10Results;
      delete[] temperatureResults;
      delete[] humidityResults;
    } 

    bool Add(uint16_t pm25, uint16_t pm10, uint8_t temp, uint8_t humid)
    {
      if(pos < arraySize)
      {
        pm25Results[pos] = pm25;
        pm10Results[pos] = pm10;
        temperatureResults[pos] = temp;
        humidityResults[pos] = humid;
        pos++;
        return true;
      }
      else
      {
        return false;
      }
    }
    void Clear()
    {
      memset(pm25Results, 0, sizeof(uint16_t) * arraySize);
      memset(pm10Results, 0, sizeof(uint16_t) * arraySize);
      memset(temperatureResults, 0, sizeof(uint8_t) * arraySize);
      memset(humidityResults, 0, sizeof(uint8_t) * arraySize);
      pos = 0;
    }

    uint16_t GetPM25Avg()
    {
      return CalcAvg(pm25Results);
    }

    uint16_t GetPM10Avg()
    {
      return CalcAvg(pm10Results);
    }

    uint16_t GetTempAvg()
    {
      return CalcAvg(temperatureResults);
    }

    uint16_t GetHumidAvg()
    {
      return CalcAvg(humidityResults);
    }
};

#define NUMBER_OF_SAMPLES   16
CSensorResults sensorResults(NUMBER_OF_SAMPLES);

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/

void setup(void)
{

#ifdef PRINT_DEBUG
  while (!Serial);
  Serial.begin(115200);
  debug(F("Adafruit Bluefruit AT Command Example"));
  debug(F("-------------------------------------"));
#endif
  delay(500);

  ble.begin(false);
  ble.echo(false);
  ble.info();
  ble.factoryReset();
  dht.begin();
  dustSensor = new CDustSensor();
  delay(500);
  if(!dustSensor->Request(dataReportingModeRequestID, DUSTSENSOR_REPORTQUERYMODE)) { debug("Request dataReportingModeRequestID failed !!!");}
  if(!dustSensor->Request(fwVersionRequestID)) { debug("Request fwVersionRequestID failed !!!");}
  if(!dustSensor->Request(setSleepWorkID, DUSTSENSOR_SETWORKMODE)) { debug("Request setSleepWorkID failed !!!");}
  
  delay(500);
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/

uint8_t goToSleepCounter = 0;

void loop(void)
{
  if(goToSleepCounter < NUMBER_OF_SAMPLES)
  {
    uint8_t h = (uint8_t)dht.readHumidity();
    uint8_t t = (uint8_t)dht.readTemperature();
  
    if(!dustSensor->Request(pmUpdateRequestID)) { debug("Request pmUpdateRequestID failed !!!");}
    uint16_t pm25 = dustSensor->GetPM25();
    uint16_t pm10 = dustSensor->GetPM10();

    if(!sensorResults.Add(pm25, pm10, t, h)) { debug("Fail - sensor queue overflow !!!"); }

    debug("Sample no " + String(goToSleepCounter));
    debug("Temp = " + String(t) + " Hum = " + String(h) + "%");
    debug("PM10 = " + String(pm10) + " PM2.5 = " + String(pm25));
  
    goToSleepCounter++;
  
    delay(2000);
  }
  else if(goToSleepCounter == NUMBER_OF_SAMPLES)
  {
    uint16_t pm25Avg = sensorResults.GetPM25Avg();
    uint16_t pm10Avg = sensorResults.GetPM10Avg();
    uint8_t tempAvg = sensorResults.GetTempAvg();
    uint8_t humidAvg = sensorResults.GetHumidAvg();
    uint8_t adv_data[] = {0x07, 0xff, humidAvg, tempAvg, (uint8_t)(pm25Avg >> 8), (uint8_t)(pm25Avg & 0xFF), (uint8_t)(pm10Avg >> 8), (uint8_t)(pm10Avg & 0xFF)};
    ble.setAdvData(adv_data, sizeof(adv_data));

    debug("Avg Temp = " + String(tempAvg));
    debug("Avg Hum = " + String(humidAvg) + "%");
    debug("Avg PM10 = " + String(pm10Avg));
    debug("Avg PM2.5 = " + String(pm25Avg)); 

    sensorResults.Clear();
    
    goToSleepCounter++;
  }
  else
  {
    goToSleepCounter = 0;
    debug("Go to sleep");
    if(!dustSensor->Request(setSleepWorkID, DUSTSENSOR_SETSLEEPMODE)) { debug("Request setSleepWorkID failed !!!");}
    delay(60000);
    debug("Wake up");
    if(!dustSensor->Request(setSleepWorkID, DUSTSENSOR_SETWORKMODE)) { debug("Request setSleepWorkID failed !!!");}
    delay(30000);
  }
}

