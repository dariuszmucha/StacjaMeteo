#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

#include "BluefruitConfig.h"
#include "BluefruitLECustom.h"

// Create the bluefruit object, either software serial...uncomment these lines
/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
CBluefruitLECustom ble = CBluefruitLECustom();

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  //while (!Serial);  // required for Flora & Micro
  delay(500);

  ble.begin(false);
  ble.echo(false);
  ble.info();

  uint8_t adv_data[] = {0x0d, 0xff, 0x04, 0x00, 0x44, 0x75, 0x70, 0x61, 0x20, 0x63, 0x79, 0x63, 0x6b, 0x69};
  ble.setAdvData(adv_data, sizeof(adv_data));
  ble.setAdvInterval(2000);
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{

}

