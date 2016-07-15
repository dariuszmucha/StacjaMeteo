#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "DHT.h"

#include "BluefruitConfig.h"
#include "BluefruitLECustom.h"

// Create the bluefruit object, either software serial...uncomment these lines
/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
CBluefruitLECustom ble = CBluefruitLECustom();

#define DHTPIN 14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define PRINT_DEBUGx
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
  while (!Serial);  // required for Flora & Micro
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

  uint8_t adv_data[] = {0x07, 0xff, h, t, 0x00, 0x00, 0x00, 0x00};
  ble.setAdvData(adv_data, sizeof(adv_data));
  
  debug(resTemp);
  debug(resHum);

  delay(60000);
}

