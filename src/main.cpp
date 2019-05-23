/*************************************************** 
    Copyright (C) 2016  Steffen Ochs, Phantomias2006

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

// EXECPTION LIST
// https://links2004.github.io/Arduino/dc/deb/md_esp8266_doc_exception_causes.html
// WATCHDOG
// https://techtutorialsx.com/2017/01/21/esp8266-watchdog-functions/

// Entwicklereinstellungen
#define DEBUG 1                               // ENABLE SERIAL DEBUG MESSAGES
//#define OTA                                 // ENABLE OTA UPDATE
//#define THINGSPEAK                          // ENABLE THINGSPEAK CONNECTION
//#define MEMORYCLOUD                         // ENABLE DATA LOGGER
//#define WEBSOCKET                           // ENABLE WEBSOCKET
//#define AMPERE                              // ENABLE AMPERE MEASUREMENT TEST
 
#ifdef DEBUG
  #define DPRINT(...)    Serial.print(__VA_ARGS__)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
  #define DPRINTP(...)   Serial.print(F(__VA_ARGS__))
  #define DPRINTPLN(...) Serial.println(F(__VA_ARGS__))
  #define DPRINTF(...)   Serial.printf(__VA_ARGS__)
  #define IPRINT(...)    Serial.print("[INFO]\t");Serial.print(__VA_ARGS__)
  #define IPRINTLN(...)  Serial.print("[INFO]\t");Serial.println(__VA_ARGS__)
  #define IPRINTP(...)   Serial.print("[INFO]\t");Serial.print(F(__VA_ARGS__))
  #define IPRINTPLN(...) Serial.print("[INFO]\t");Serial.println(F(__VA_ARGS__))
  #define IPRINTF(...)   Serial.print("[INFO]\t");Serial.printf(__VA_ARGS__)
#else
  #define DPRINT(...)     //blank line
  #define DPRINTLN(...)   //blank line 
  #define DPRINTP(...)    //blank line
  #define DPRINTPLN(...)  //blank line
  #define DPRINTF(...)    //blank line
  #define IPRINT(...)     //blank line
  #define IPRINTLN(...)   //blank line
  #define IPRINTP(...)    //blank line
  #define IPRINTPLN(...)  //blank line
  #define IPRINTF(...)    //blank line
#endif

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
// INCLUDE SUBROUTINES
#include <vector>
#include "c_init.h"
#include "c_median.h"
#include "c_sensor_mini.h"
#include "c_pitmaster_mini.h"
#include "c_temp.h"
#include "c_ee.h"
#include "c_fs.h"
#include "c_icons.h"
#include "c_wifi.h"
#include "c_display.h"
#include "c_webhandler.h"
#include "c_com.h"
#include "c_bot.h"
#include "c_pmqtt.h"
#include "c_ota.h"
#include "c_server.h"
#include "c_api.h"
#include "c_ws.h"
 
// Forward declaration
void createTasks();


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SETUP
void setup() {  

  // Initialize Serial 
  set_serial(); Serial.setDebugOutput(true);
  printf("SDK version:%s\n", system_get_sdk_version()); //getSdkVersion

  // Initialize serial number
  sprintf(serialnumber, "%06x", ESP.getEfuseMac());

  // Initialize display
  set_display();

  // Open Config-File
  check_sector();
  setEE(); start_fs();

  get_rssi();

  if (!sys.stby) {
    
    // Initalize Aktor
    set_piepser();

    // GodMode aktiv
    if (sys.god & (1<<0)) {
      piepserON(); delay(500); piepserOFF();
    }

    // Initialize mqtt
    set_pmqtt(1);
    
    // Initialize Wifi
    set_wifi();

    // Initialize Server
    server_setup();
    
    // Initialize OTA
    #ifdef OTA  
      set_ota();
      ArduinoOTA.begin();
    #endif
    
    // Initialize Sensors
    set_sensor();
    set_channels(0);
        
    // Current Wifi Signal Strength
    get_rssi();
    
    // Initialize Pitmaster
    set_pitmaster(0); 

    // Start all tasks
    createTasks();
  }
}


void MainTask( void * parameter ) {

  TickType_t xLastWakeTime = xTaskGetTickCount();

  for(;;) {

    // Wait for the next cycle.
    vTaskDelayUntil(&xLastWakeTime, 100);

    // Detect Serial Input
    static char serialbuffer[300];
    if (readline(Serial.read(), serialbuffer, 300) > 0) {
      read_serial(serialbuffer);
    }

    // Manual Restart
    if (sys.restartnow) {
      if (wifi.mode == 5)
        WiFi.disconnect();
      delay(100);
      yield();
      ESP.restart();
    }

    // WiFi - Monitoring
    wifimonitoring();

    // MQTT - Abschaltung und Initialisierung
    checkMqtt(); 

    // Detect OTA
    #ifdef OTA
      ArduinoOTA.handle();
    #endif

    // HTTP Update
    check_api();
    if (update.state > 0)
      do_http_update();

  }
}

void TemperatureTask( void * parameter )
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  uint8_t sensorIndex = 0;

  for(;;) {

    // Wait for the next cycle.
    vTaskDelayUntil(&xLastWakeTime, 50);

    get_Temperature(sensorIndex);
    controlAlarm(sensorIndex);

    sensorIndex++;
    if(sensorIndex == sys.ch)
      sensorIndex = 0;

  }
}

void PitmasterTask( void * parameter )
{
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for(;;) {

    // Wait for the next cycle.
    vTaskDelayUntil(&xLastWakeTime, 100);
    pitmaster_control(0);
    pitmaster_control(1);

  }
}

void ConnectTask( void * parameter )
{
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for(;;) {

    // Wait for the next cycle.
    vTaskDelayUntil(&xLastWakeTime, 1000);

    if (wifi.mode == 1 && update.state == 0 && iot.P_MQTT_on)
      sendpmqtt();

    if (wifi.mode == 1 && update.state == 0)
      sendNotification();
    
    // NANO CLOUD (nach Notification)
    if (lastUpdateCloud) {
      if (wifi.mode == 1 && update.state == 0 && iot.CL_on) {
        if (sendAPI(0)) {
          apiindex = APICLOUD;
          urlindex = CLOUDLINK;
          parindex = NOPARA;
          sendAPI(2);
        } else {
          #ifdef MEMORYCLOUD  
            cloudcount = 0;           // ansonsten von API zurückgesetzt
          #endif 
        }
      }
      lastUpdateCloud = false;
    }

    // ALARM REPEAT
      if (1) {     // 15 s
       for (int i=0; i < sys.ch; i++) {
        if (ch[i].isalarm)  {
          if (ch[i].repeat > 1) {
            ch[i].repeat -= 1;
            ch[i].repeatalarm = true;
          }
        } else ch[i].repeat = pushd.repeat;
       }
      }
  }
}

void createTasks()
{
    xTaskCreate(
              TemperatureTask,  /* Task function. */
              "TemperatureTask",/* String with name of task. */
              10000,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              0,                /* Priority of the task. */
              NULL);            /* Task handle. */

    xTaskCreate(
              PitmasterTask,    /* Task function. */
              "PitmasterTask",  /* String with name of task. */
              10000,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              1,                /* Priority of the task. */
              NULL);            /* Task handle. */

    xTaskCreate(
              MainTask,         /* Task function. */
              "MainTask",       /* String with name of task. */
              10000,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              2,                /* Priority of the task. */
              NULL);            /* Task handle. */

    xTaskCreate(
              ConnectTask,      /* Task function. */
              "ConnecTask",     /* String with name of task. */
              10000,            /* Stack size in bytes. */
              NULL,             /* Parameter passed as input of the task */
              3,                /* Priority of the task. */
              NULL);            /* Task handle. */
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// LOOP
void loop() {
  delay(1000);
}


