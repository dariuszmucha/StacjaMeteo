#include "BluefruitLECustom.h"
#include "BluefruitConfig.h"

CBluefruitLECustom::CBluefruitLECustom()
: Adafruit_BluefruitLE_SPI(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST)
{ }

void CBluefruitLECustom::setAdvInterval()
{
  atcommand(F("AT+GAPINTERVALS=,,500,"));
}

