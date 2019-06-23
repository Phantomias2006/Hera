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
#pragma once

#include "Arduino.h"

#define INACTIVEVALUE 999

class TemperatureBase
{
  public:
    TemperatureBase();
    float getValue();
    int getMinValue();
    int getMaxValue();
    uint8_t getCount();
    void getType();
    void setType(uint8_t type);
    void setMinValue(int value);
    void setMaxValue(int value);
    boolean hasAlarm();
    void virtual update();
  protected:
    uint8_t localIndex;
    float currentValue;
    int minValue;
    int maxValue;
    uint8_t type;
};
