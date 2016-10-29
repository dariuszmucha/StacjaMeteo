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

}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  uint8_t h = (uint8_t)dht.readHumidity();
  // Read temperature as Celsius (the default)
  uint8_t t = (uint8_t)dht.readTemperature();

  String resTemp = "Temp = " + String(t);
  String resHum = "Hum = " + String(h) + "%";

  dustSensor->Update();
  uint16_t pm25 = dustSensor->GetPM25();
  uint16_t pm10 = dustSensor->GetPM10();

  uint8_t adv_data[] = {0x07, 0xff, h, t, (uint8_t)(pm25 >> 8), (uint8_t)(pm25 & 0xFF), (uint8_t)(pm10 >> 8), (uint8_t)(pm10 & 0xFF)};
  ble.setAdvData(adv_data, sizeof(adv_data));
  
  debug(resTemp);
  debug(resHum);
  debug("PM10 = " + String(pm10));
  debug("PM2.5 = " + String(pm25));

  delay(2000);
}

