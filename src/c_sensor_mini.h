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

    ANALOG/DIGITAL-WANDLUNG:
    - kleinster digitaler Sprung 1.06 V/1024 = 1.035 mV - eigentlich 1.0V/1024
    - Hinweis zur Abweichung: https://github.com/esp8266/Arduino/issues/2672
    -> ADC-Messspannung = Digitalwert * 1.035 mV
    - Spannungsteiler (47k / 10k) am ADC-Eingang zur 
    - Transformation von BattMin - BattMax in den Messbereich von 0 - 1.06V 
    -> Batteriespannung = ADC-Messspannung * (47+10)/10 
    -> Transformationsvariable Digitalwert-to-Batteriespannung: Battdiv = 1.035 mV * 5.7
    
    HISTORY: Please refer Github History
    
 ****************************************************/

// Global variables
MCP3208 adc(ADC_VREF, SPI_CS);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Sensors
void set_sensor() {

  // disable battery
  sys.hasBattery = false;

   // configure PIN mode
  pinMode(SPI_CS, OUTPUT);

  // set initial PIN state
  digitalWrite(SPI_CS, HIGH);

  // initialize SPI interface for MCP3208
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Reading ADC-Channel Average
int get_adc_average (uint8_t ch) {  

  MCP3208::Channel mapping[8] = {MCP3208::SINGLE_0, MCP3208::SINGLE_1, MCP3208::SINGLE_2, MCP3208::SINGLE_3,
                                 MCP3208::SINGLE_4, MCP3208::SINGLE_5, MCP3208::SINGLE_6, MCP3208::SINGLE_7};
  word regdata = adc.read(mapping[ch]);

  return regdata & 4095;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Hardware Alarm
void set_piepser() {

  // Hardware-Alarm bereit machen
  pinMode(PIEPSER, OUTPUT);
  analogWriteFrequency(PIEPSER, 1000);
}

void piepserON() {
  analogWrite(PIEPSER, 512);
  sys.piepoff_t = 2;
}

void piepserOFF() {
  if (sys.piepoff_t == 0) analogWrite(PIEPSER,0);
  else if (sys.piepoff_t > -2) sys.piepoff_t--;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Control Hardware Alarm
void controlAlarm(uint8_t index){                // action dient zur Pulsung des Signals

  uint8_t i = index;
  bool setalarm = false;
              
  // CHECK LIMITS
  if ((ch[i].temp <= ch[i].max && ch[i].temp >= ch[i].min) || ch[i].temp == INACTIVEVALUE) {
    // everything is ok
    ch[i].isalarm = false;                      // no alarm
    ch[i].showalarm = 0;                    // no OLED note
    notification.index &= ~(1<<i);              // delete channel from index
    //notification.limit &= ~(1<<i);

  } else if (ch[i].isalarm && ch[i].showalarm > 0) {  // Alarm noch nicht quittiert
    // do summer alarm
    setalarm = true;                            

    // show alarm on display
    //TODO
  
  } else if ((!ch[i].isalarm || ch[i].repeatalarm) && ch[i].temp != INACTIVEVALUE) {
    // first rising limits

    ch[i].isalarm = true;                      // alarm

    if (ch[i].alarm == 1 || ch[i].alarm == 3) {   // push or all
      notification.index &= ~(1<<i);
      notification.index |= 1<<i;                // Add channel to index   
    
      if (ch[i].temp > ch[i].max) {
        notification.limit |= 1<<i;              // add upper limit
      }
      else if (ch[i].temp < ch[i].min) { 
        notification.limit &= ~(1<<i);           // add lower limit              
      }
    } 

    ch[i].repeatalarm = false;
    
    if (ch[i].alarm > 1) {                       // only if summer
      ch[i].showalarm = 2;                    // show OLED note first time
    }
  }

  // Hardware-Alarm-Variable: sys.hwalarm
  if (setalarm) piepserON(); 
}