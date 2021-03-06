#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "DHT.h"
#include "BluefruitConfig.h"
#include "BluefruitLECustom.h"
#include "CDustSensor.h"
#include "CBattery.h"
#include "CSensorResults.h"

CBluefruitLECustom ble = CBluefruitLECustom();

#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define NUMBER_OF_SAMPLES   16
CSensorResults sensorResults(NUMBER_OF_SAMPLES);

#define VBATPIN       A7
CBattery batteryLevel(VBATPIN);

CDustSensor *dustSensor;

#define PRINT_DEBUG
#ifdef PRINT_DEBUG
#define debug(x) Serial.println(x)
#else
#define debug
#endif

#define SLEEPPERIOD 600 // [s]
#define WAKEUPDELAY 60  // [s]
#define WORKTICK    2   // [s]

void delaySeconds(uint16_t seconds)
{
  while(seconds-- > 0)
  {
    delay(1000);
  }
}

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

static void GoToSleepDustSensor()
{
  debug("Go to sleep");
  if(!dustSensor->Request(setSleepWorkID, DUSTSENSOR_SETSLEEPMODE)) { debug("Request setSleepWorkID failed !!!");}
  delaySeconds(SLEEPPERIOD);
}

static void WakeUpDustSensor()
{
  debug("Wake up");
  if(!dustSensor->Request(setSleepWorkID, DUSTSENSOR_SETWORKMODE)) { debug("Request setSleepWorkID failed !!!");}
  delaySeconds(WAKEUPDELAY);
}

static void UpdateAdvData()
{
  uint16_t pm25Avg = sensorResults.GetPM25Avg();
  uint16_t pm10Avg = sensorResults.GetPM10Avg();
  int8_t tempAvg = sensorResults.GetTempAvg();
  uint8_t humidAvg = sensorResults.GetHumidAvg();
  

  debug("Avg Temp = " + String(tempAvg));
  debug("Avg Hum = " + String(humidAvg) + "%");
  debug("Avg PM10 = " + String(pm10Avg));
  debug("Avg PM2.5 = " + String(pm25Avg)); 

  sensorResults.Clear();

  uint8_t battLevel = batteryLevel.GetBattLevel();
  debug("VBat: " + String(battLevel) + "%"); 

  uint8_t adv_data[] = {0x08, 0xff, humidAvg, tempAvg, (uint8_t)(pm25Avg >> 8), (uint8_t)(pm25Avg & 0xFF), (uint8_t)(pm10Avg >> 8), (uint8_t)(pm10Avg & 0xFF), battLevel};
  ble.setAdvData(adv_data, sizeof(adv_data));
}

static void GetDataFromSensors()
{
  uint8_t h = (uint8_t)dht.readHumidity();
  int8_t t = (int8_t)dht.readTemperature();
  uint16_t pm25 = 0;
  uint16_t pm10 = 0;
  do
  {
    if(!dustSensor->Request(pmUpdateRequestID)) { debug("Request pmUpdateRequestID failed !!!");}
    pm25 = dustSensor->GetPM25();
    pm10 = dustSensor->GetPM10();
  } while(pm25 == 0 || pm10 == 0);

  if(!sensorResults.Add(pm25, pm10, t, h)) { debug("Fail - sensor queue overflow !!!"); }

  debug("Sample no " + String(goToSleepCounter));
  debug("Temp = " + String(t) + " Hum = " + String(h) + "%");
  debug("PM10 = " + String(pm10) + " PM2.5 = " + String(pm25));

  delaySeconds(WORKTICK);
}

void loop(void)
{
  if(batteryLevel.GetBattLevel() > 0)
  {
    if(goToSleepCounter < NUMBER_OF_SAMPLES)
    {
      GetDataFromSensors();
      goToSleepCounter++;
    }
    else if(goToSleepCounter == NUMBER_OF_SAMPLES)
    {
      UpdateAdvData();
      goToSleepCounter++;
    }
    else
    {
      goToSleepCounter = 0;
      GoToSleepDustSensor();
      WakeUpDustSensor();
    }
  }
  else
  {
    debug("Battery level critical");
    GoToSleepDustSensor();
  }
}

