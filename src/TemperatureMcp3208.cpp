/*************************************************** 
    Copyright (C) 2016  Steffen Ochs
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

#include "TemperatureMcp3208.h"
#include <SPI.h>

union SplitTwoBytes
{
  uint16_t value;
  struct
  {
    uint8_t lowByte;
    uint8_t highByte;
  };
};

TemperatureMcp3208::TemperatureMcp3208()
{

}

TemperatureMcp3208::TemperatureMcp3208(uint8_t index, uint8_t csPin)
{
    this->localIndex = index;
    this->csPin = csPin;
}

void TemperatureMcp3208::update()
{
    this->currentValue = this->calcTemperature(this->readChip());
}

uint16_t TemperatureMcp3208::readChip()
{
    SplitTwoBytes receive;
    SplitTwoBytes command;
    command.value = 0x0400;

    // set channel
    command.value |= ((this->localIndex + 8u) << 6u);

    // write CS
    digitalWrite(csPin, LOW);

    // send first byte
    SPI.transfer(command.highByte);
    // send second byte and receive first 4 bits
    receive.highByte = SPI.transfer(command.lowByte) & 0x0Fu;
    // receive last 8 bits
    receive.lowByte = SPI.transfer(0x00u);

    // write CS
    digitalWrite(csPin, HIGH);
    
    return receive.value;
}

float TemperatureMcp3208::calcTemperature(uint16_t rawValue)
{ 

  float Rmess = 47;
  float a, b, c, Rn;

  // kleine Abweichungen an GND verursachen Messfehler von wenigen Digitalwerten
  // daher werden nur Messungen mit einem Digitalwert von mind. 10 ausgewertet,
  // das entspricht 5 mV
  if (rawValue < 10) return INACTIVEVALUE;        // Kanal ist mit GND gebrückt

  switch (this->type) {
  case 0:  // Maverik
    Rn = 1000; a = 0.003358; b = 0.0002242; c = 0.00000261;
    break; 
  case 1:  // Fantast-Neu
    Rn = 220; a = 0.00334519; b = 0.000243825; c = 0.00000261726;
    break; 
  case 2:  // Fantast
    Rn = 50.08; a = 3.3558340e-03; b = 2.5698192e-04; c = 1.6391056e-06;
    break; 
  case 3:  // iGrill2
    Rn = 99.61 ; a = 3.3562424e-03; b = 2.5319218e-04; c = 2.7988397e-06;
    break; 
  case 4:  // ET-73
    Rn = 200; a = 0.00335672; b = 0.000291888; c = 0.00000439054; 
    break;
  case 5:  // PERFEKTION
    Rn = 200.1; a =  3.3561990e-03; b = 2.4352911e-04; c = 3.4519389e-06;  
    break; 
  case 6:  // 50K 
    Rn = 50.0; a = 3.35419603e-03; b = 2.41943663e-04; c = 2.77057578e-06;
    break; 
  case 7: // INKBIRD
    Rn = 48.59; a = 3.3552456e-03; b = 2.5608666e-04; c = 1.9317204e-06;
    //Rn = 48.6; a = 3.35442124e-03; b = 2.56134397e-04; c = 1.9536396e-06;
    //Rn = 48.94; a = 3.35438959e-03; b = 2.55353377e-04; c = 1.86726509e-06;
    break;
  case 8: // NTC 100K6A1B (lila Kopf)
    Rn = 100; a = 0.00335639; b = 0.000241116; c = 0.00000243362; 
    break;
  case 9: // Weber_6743
    Rn = 102.315; a = 3.3558796e-03; b = 2.7111149e-04; c = 3.1838428e-06; 
    break;
  case 10: // Santos
    Rn = 200.82; a = 3.3561093e-03; b = 2.3552814e-04; c = 2.1375541e-06; 
    break;
  case 11: // NTC 5K3A1B (orange Kopf)
    Rn = 5; a = 0.0033555; b = 0.0002570; c = 0.00000243; 
    break;
  default:  
    return INACTIVEVALUE;
  }
  
  float Rt = Rmess*((4096.0/(4096-rawValue)) - 1);
  float v = log(Rt/Rn);
  float erg = (1/(a + b*v + c*v*v)) - 273.15;
  
  return (erg>-31)?erg:INACTIVEVALUE;
}