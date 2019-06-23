/*************************************************** 
    Copyright (C) 2019  Martin Koerner

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    HISTORY: Please refer Github History
    
****************************************************/

#include <SPI.h>
#include "GrpTemperatureMiniV2.h"
#include "TemperatureMcp3208.h"
#include "TemperatureMax31855.h"

#define CS_MCP3208  21u
#define CS_MAX31855_N1  33u
#define CS_MAX31855_N2  13u
#define ADC_CLK 1600000  // SPI clock 1.6MHz

GrpTemperature::GrpTemperature()
{
     // configure PIN mode
  pinMode(CS_MCP3208, OUTPUT);
  pinMode(CS_MAX31855_N1, OUTPUT);
  pinMode(CS_MAX31855_N2, OUTPUT);

  // set initial PIN state
  digitalWrite(CS_MCP3208, HIGH);
  digitalWrite(CS_MAX31855_N1, HIGH);
  digitalWrite(CS_MAX31855_N2, HIGH);

  // initialize SPI interface
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);

  // initialize temperature array
  temperature[0u] = new TemperatureMcp3208(0u, CS_MCP3208);
  temperature[1u] = new TemperatureMcp3208(1u, CS_MCP3208);
  temperature[2u] = new TemperatureMcp3208(2u, CS_MCP3208);
  temperature[3u] = new TemperatureMcp3208(3u, CS_MCP3208);
  temperature[4u] = new TemperatureMcp3208(4u, CS_MCP3208);
  temperature[5u] = new TemperatureMcp3208(5u, CS_MCP3208);
  temperature[6u] = new TemperatureMcp3208(6u, CS_MCP3208);
  temperature[7u] = new TemperatureMcp3208(7u, CS_MCP3208);
  temperature[8u] = new TemperatureMax31855(CS_MAX31855_N1);
  temperature[9u] = new TemperatureMax31855(CS_MAX31855_N2);
  temperature[10u] = NULL;
  temperature[11u] = NULL;
}

TemperatureBase *GrpTemperature::operator[] (int i)
{
  TemperatureBase* p = NULL;

  if(i < MAX_TEMPERATURES_MINIV2)
    p = temperature[i];

  return p;
}
