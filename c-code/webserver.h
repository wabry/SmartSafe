#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Arduino.h"

struct timeStruct{
  char dayOfWeek; // Sunday - Saturday -> 0-6
  char month; // Jan - Dec -> 0-11
  int day;
  int year;
  int hour;
  int minute;
  int second;
};

void saveTimeToEEPROM();
void loadTimeFromEEPROM();
void updateWebpage();
String getTime();
timeStruct fillTime();
void convertDayOfWeek(timeStruct* obj, String stringIn);
void convertMonth(timeStruct* obj, String stringIn);
String getDayOfWeek(timeStruct* obj);
String getMonth(timeStruct* obj);
void storeTime(int addr,timeStruct* obj);
void loadTime(int addr,timeStruct* obj);
String timeToStr(timeStruct* obj);


#endif //WEBSERVER_H