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
#ifndef C_DISPLAY_H_
#define C_DISPLAY_H_

#define DONT_CARE 0
#define MAIN_PAGE_ID 0
#define WLANINFO_PAGE_ID 18

NexUpload nexUpload(115200);

static bool nextion_init_failed = false;

static NexButton button_min_temp = NexButton(6, 15, "b12");
static NexButton button_max_temp = NexButton(7, 15, "b12");

static NexVariable temp_alarm = NexVariable(2, DONT_CARE, "temp\.alarm");
static NexVariable main_timeout = NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.timeout");
static NexVariable wlaninfo_wlan0 = NexVariable(WLANINFO_PAGE_ID, DONT_CARE, "wlaninfo\.wlan0");
static NexVariable main_signal = NexVariable(WLANINFO_PAGE_ID, DONT_CARE, "main\.signal");

static NexVariable sensor_type[8] = { NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type0"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type1"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type2"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type3"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type4"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type5"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type6"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type7")};

static NexVariable kanal[8] =       { NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal0"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal1"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal2"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal3"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal4"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal5"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal6"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal7")};

static NexVariable al_min_ist[12] = { NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al0minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al1minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al2minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al3minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al4minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al5minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al6minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al7minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al8minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al9minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al10minist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al11minist")};

static NexVariable al_max_ist[12] = { NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al0maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al1maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al2maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al3maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al4maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al5maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al6maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al7maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al8maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al9maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al10maxist"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al11maxist")};

static NexTouch *nex_listen_list[] = {
  &button_min_temp,
  &button_max_temp,
  NULL
};

void buttonSetMinTemp(void *ptr) {
  char text[10];
  uint32_t i = 0;

  if(temp_alarm.getValue(&i))
  {
    if(al_min_ist[i].getText(text, sizeof(text)))
    {
      ch[i].min = atof(text);
    }
  }
}

void buttonSetMaxTemp(void *ptr) {
  char text[10];
  uint32_t i = 0;

  if(temp_alarm.getValue(&i))
  {
    if(al_max_ist[i].getText(text, sizeof(text)))
    {
      ch[i].max = atof(text);
    }
  }
}

// Initialize Display
void set_display() {
    char text[10];

    if(nexInit())
    {
      button_min_temp.attachPop(buttonSetMinTemp, &button_min_temp);
      button_max_temp.attachPop(buttonSetMaxTemp, &button_max_temp);
      delay(1000);
      sendCommand("page 1");

      main_timeout.setValue(0);

      for (int i=0; i < sys.ch; i++) {
        sprintf(text, "%.f", ch[i].min);
        al_min_ist[i].setText(text);
        sprintf(text, "%.f", ch[i].max);
        al_max_ist[i].setText(text);
      }

    }
    else {
      Serial.printf("Cannot connect to nextion display. Try to upload tft file!\n");
      nextion_init_failed = true;
    }
}

int update_display() {
    uint32_t value = 0;
    char text[10];

    if(nexUpload.isUploading() || nextion_init_failed)
      return 1;

    nexLoop(nex_listen_list);

    /*if(sensor_type[0].getValue(&value))
    {
        ch[0].typ = value;
    }*/

  for (int i=0; i < sys.ch; i++) {
    if (ch[i].temp!=INACTIVEVALUE) {
      sprintf(text, "%.1lf\xb0%s", ch[i].temp, sys.unit.c_str());
    }
    else {
      sprintf(text, "");
    }
    kanal[i].setText(text);
  }

  wlaninfo_wlan0.setText(WiFi.localIP().toString().c_str());

  return 1;
}

// handler for upload of nextion tft file
void onNexUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
 
  //Handle upload
  if(!index)
  {
    Serial.printf("UploadStart: %s\n", filename.c_str());
    // prepare nextion display for upload
    nexUpload.prepareUpload();
  }

  // send data to nextion display
  nexUpload.upload(data, len);

  if(final)
  {
    Serial.printf("UploadEnd: %s (%u bytes)\n", filename.c_str(), index + len);
    // restart uC after upload
    //ESP.restart();
  }
}

#endif /* C_CONSTS_H_ */