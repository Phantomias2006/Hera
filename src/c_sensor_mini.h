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

MCP3208 adc(ADC_VREF, SPI_CS);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Sensors
void set_sensor() {

  // disable battery
  sys.hasBattery = false;

  // Piepser
  //pinMode(MOSI, OUTPUT);
  analogWriteFrequency(4000);

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
int get_adc_average (byte ch) {  

  MCP3208::Channel mapping[8] = {MCP3208::SINGLE_0, MCP3208::SINGLE_1, MCP3208::SINGLE_2, MCP3208::SINGLE_3,
                                 MCP3208::SINGLE_4, MCP3208::SINGLE_5, MCP3208::SINGLE_6, MCP3208::SINGLE_7};
  word regdata = adc.read(mapping[ch]);

  return regdata & 4095;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Charge Detection
void set_batdetect(boolean stat) {
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Reading Battery Voltage
void get_Vbat() {
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Calculate SOC
void cal_soc() {
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Hardware Alarm
void set_piepser() {

  // Hardware-Alarm bereit machen
  pinMode(PIEPSER, OUTPUT);
  analogWriteFrequency(4000);
  //sys.hwalarm = false;

}

void piepserON() {
  analogWrite(PIEPSER, 512);
  sys.piepoff_t = 2;
}

void piepserOFF() {
  if (sys.piepoff_t == 0) analogWrite(PIEPSER,0);
  else if (sys.piepoff_t > -2) sys.piepoff_t--;
}

void pbguard() {
  //analogWriteFreq(5);
  analogWrite(PIEPSER,1023);
  sys.piepoff_t = 2;        // für 2 Zyklen
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Control Hardware Alarm
void controlAlarm(){                // action dient zur Pulsung des Signals

  bool setalarm = false;

  for (int i=0; i < sys.ch; i++) {
    //if (ch[i].alarm > 0) {                              // CHANNEL ALARM ENABLED
                
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

        // Show Alarm on OLED
        if (ch[i].showalarm == 2 && !displayblocked) {    // falls noch nicht angezeigt
          ch[i].showalarm = 1;                            // nur noch Summer
          question.typ = HARDWAREALARM;
          question.con = i;
          drawQuestion(i);
        }
      
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
    //} else {                                      // CHANNEL ALARM DISABLED
    //  ch[i].isalarm = false;
    //  ch[i].showalarm = 0;
    //  notification.index &= ~(1<<i);              // delete channel from index
      //notification.limit &= ~(1<<i);
    //}   
  }

  // Hardware-Alarm-Variable: sys.hwalarm
  if (setalarm) piepserON(); 
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Reading Temperature KTYPE
double get_thermocouple(bool internal) {

  long dd = 0;
  
  // Communication per I2C Pins but with CS
  digitalWrite(THERMOCOUPLE_CS, LOW);                    // START
  for (uint8_t i=32; i; i--){
    dd = dd <<1;
    //@MK if (twi_read_bit())  dd |= 0x01;                     // needs #include "core_esp8266_si2c.c"
  }
  digitalWrite(THERMOCOUPLE_CS, HIGH);                   // END

  // Invalid Measurement
  if (dd & 0x7) {             // Abfrage von D0/D1/D2 (Fault)
    return INACTIVEVALUE; 
  }

  if (internal) {
    // Internal Reference
    double ii = (dd >> 4) & 0x7FF;     // Abfrage D4-D14
    ii *= 0.0625;
    if ((dd >> 4) & 0x800) ii *= -1;  // Abfrage D15 (Vorzeichen)
    return ii;
  }
  

  // Temperatur
  if (dd & 0x80000000) {    // Abfrage D31 (Vorzeichen)
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    dd = 0xFFFFC000 | ((dd >> 18) & 0x00003FFFF);
  }
  else {
    // Positive value, just drop the lower 18 bits.
    dd >>= 18;
  }

  // Temperature in Celsius
  double vv = dd;
  vv *= 0.25;
  return vv;
}