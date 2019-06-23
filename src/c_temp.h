 /*************************************************** 
    Copyright (C) 2016  Steffen Ochs

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

#include "GrpTemperatureMiniV2.h"

GrpTemperature* Temperatures = new GrpTemperature();

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Reading Temperature
void get_Temperature(uint8_t index) {

  uint8_t i = index;

  // Read channel
  float value = INACTIVEVALUE;
  TemperatureBase *temperature = (*Temperatures)[i];
  if(temperature != NULL)
  {
    temperature->setType(ch[i].typ);
    temperature->update();
    value = temperature->getValue();
  }

  // Temperatursprung außerhalb der Grenzen macht keinen Sinn
  if (ch[i].temp == INACTIVEVALUE && (value < -15.0 || value > 300.0)) value = INACTIVEVALUE;  // wrong typ

  // Umwandlung C/F
  if ((sys.unit == "F") && value!=INACTIVEVALUE) {  // Vorsicht mit INACTIVEVALUE
    value *= 9.0;
    value /= 5.0;
    value += 32;
  }

  // Temperature Average Buffer by Pitmaster
  if (sys.transform) {
    if (value != INACTIVEVALUE) {
      mem_add(value, i);
    } else {
      mem_clear(i);
    }
    value = mem_a(i);
  }
  
  ch[i].temp = value;
  
  int max = ch[i].max;  // nur für Anzeige
  int min = ch[i].min;  // nur für Anzeige

  // Open Lid Detection
  open_lid();

}

void get_TemperatureAll()
{
  for(uint8_t i = 0; i < sys.ch; i++)
    get_Temperature(i);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Temperature Channels
void set_channels(bool init) {

  // Grundwerte einrichten
  for (int i=0; i<sys.ch; i++) {
        
    ch[i].temp = INACTIVEVALUE;
    ch[i].match = 0;
    ch[i].isalarm = false;
    ch[i].showalarm = 0;

    if (init) {
      ch[i].name = ("Kanal " + String(i+1));
      ch[i].typ = 0;
    
      if (sys.unit == "F") {
        ch[i].min = ULIMITMINF;
        ch[i].max = OLIMITMINF;
      } else {
        ch[i].min = ULIMITMIN;
        ch[i].max = OLIMITMIN;
      }
  
      ch[i].alarm = false; 
      ch[i].color = colors[i];
    }
  }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Transform Channel Limits
void transform_limits() {
  
  float max;
  float min;
  
  for (int i=0; i < sys.ch; i++)  {
    max = ch[i].max;
    min = ch[i].min;

    if (sys.unit == "F") {               // Transform to °F
      max *= 9.0;
      max /= 5.0;
      max += 32;
      min *= 9.0;
      min /= 5.0;
      min += 32; 
    } else {                              // Transform to °C
      max -= 32;
      max *= 5.0;
      max /= 9.0;
      min -= 32;
      min *= 5.0;
      min /= 9.0;
    }

    ch[i].max = max;
    ch[i].min = min;
  }
}

