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

#include "TemperatureBase.h"

TemperatureBase::TemperatureBase()
{
  this->currentValue = INACTIVEVALUE;
}

float TemperatureBase::getValue()
{
  return this->currentValue;
}

int TemperatureBase::getMinValue()
{
  return this->minValue;
}

int TemperatureBase::getMaxValue()
{
  return this->maxValue;
}

void TemperatureBase::setType(uint8_t type)
{
  this->type = type;
}

void TemperatureBase::setMinValue(int value)
{
  this->minValue = value;
}

void TemperatureBase::setMaxValue(int value)
{
  this->maxValue = value;
}

boolean TemperatureBase::hasAlarm()
{
  boolean alarm = false;

  if(INACTIVEVALUE == this->currentValue)
    alarm = false;
  else if(this->currentValue <= this->minValue)
    alarm = true;
  else if(this->currentValue >= this->maxValue)
    alarm = true;

  return alarm;
}

void TemperatureBase::update()
{

}