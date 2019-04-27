#ifndef C_DISPLAY_H_
#define C_DISPLAY_H_

#define DONT_CARE 0
#define MAIN_PAGE_ID 0
#define WLANINFO_PAGE_ID 18

NexButton button_min_temp = NexButton(6, 15, "b12");
NexButton button_max_temp = NexButton(7, 15, "b12");

NexVariable temp_alarm = NexVariable(2, DONT_CARE, "temp\.alarm");
NexVariable main_timeout = NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.timeout");
NexVariable wlaninfo_wlan0 = NexVariable(WLANINFO_PAGE_ID, DONT_CARE, "wlaninfo\.wlan0");
NexVariable main_signal = NexVariable(WLANINFO_PAGE_ID, DONT_CARE, "main\.signal");

NexVariable sensor_type[8] = { NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type0"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type1"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type2"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type3"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type4"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type5"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type6"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.sensor_type7")};

NexVariable kanal[8] =       { NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal0"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal1"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal2"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal3"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal4"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal5"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal6"),
                               NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.kanal7")};

NexVariable al_min_ist[12] = { NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al0minist"),
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

NexVariable al_max_ist[12] = { NexVariable(MAIN_PAGE_ID, DONT_CARE, "main\.al0maxist"),
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
 
NexTouch *nex_listen_list[] = {
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

    nexInit();
    button_min_temp.attachPop(buttonSetMinTemp, &button_min_temp);
    button_max_temp.attachPop(buttonSetMaxTemp, &button_max_temp);
    sendCommand("page 1");

    Serial.print("sys.ch: ");
    Serial.println(sys.ch);

    main_timeout.setValue(0);

    for (int i=0; i < sys.ch; i++) {
      sprintf(text, "%.f", ch[i].min);
      al_min_ist[i].setText(text);
      sprintf(text, "%.f", ch[i].max);
      al_max_ist[i].setText(text);
    }
}

int update_display() {
    uint32_t value = 0;
    char text[10];

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

#endif /* C_CONSTS_H_ */