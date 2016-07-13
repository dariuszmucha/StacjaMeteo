#include "BluefruitLECustom.h"
#include "BluefruitConfig.h"

CBluefruitLECustom::CBluefruitLECustom()
: Adafruit_BluefruitLE_SPI(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST)
{ }

void CBluefruitLECustom::setAdvInterval(uint16_t interval)
{
  uint8_t advdata[] = {0x01, 0x02};
  size_t size = 2;
  atcommand(F("AT+GAPINTERVALS=,,2000,"));
}

