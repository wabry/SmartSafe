#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#include "webserver.h"

extern ESP8266WebServer server;
extern String webPage;

int logNums[LOG_COUNT];
timeStruct timeVals[LOG_COUNT];

void saveTimeToEEPROM()
{
  timeStruct store = fillTime();
  storeTime(&store);
  EEPROM.commit();
}

void loadTimeFromEEPROM()
{
  loadTime();
}

String getTimeString(int index)
{
  return timeToStr(&(timeVals[index]));
}

void updateWebpage()
{
  webPage = "<head><title>SmartSafe Webserver</title></head>";
  webPage += "<center><h1>SmartSafe User Panel </h1></center><body>";
  webPage += "<h2>Recent Openings</h2>";
  webPage += "<ol type=\"1\">";
  String timeStr;
  int nextVal = 0;
  for(size_t i=0;i<LOG_COUNT;++i)
  {
    for(int j=0;j<LOG_COUNT;++j)
    {
      if(logNums[j] == nextVal)
      {
        timeStr = getTimeString(j);
        webPage += timeStr;
        nextVal++;
        break;
      }
    }
  }
  webPage += "</ol><br>";
  webPage += "<p>View our code <a href=\"https://wabry.github.io/SmartSafe/\">here</a></p></body>";
}

timeStruct fillTime()
{
  String timeStr = getTime();
  String dayOfWeekStr = timeStr.substring(0,3);
  String dayStr = timeStr.substring(5,7);
  String monthStr = timeStr.substring(8,11);
  String yearStr = timeStr.substring(12,16);
  String hourStr = timeStr.substring(17,19);
  String minStr = timeStr.substring(20,22);
  String secStr = timeStr.substring(23,25);
  timeStruct returnStruct;
  convertDayOfWeek(&returnStruct, dayOfWeekStr);
  returnStruct.day = dayStr.toInt();
  convertMonth(&returnStruct, monthStr);
  returnStruct.year = yearStr.toInt();
  returnStruct.year -= 2000;
  returnStruct.hour = hourStr.toInt();
  returnStruct.minute = minStr.toInt();
  returnStruct.second = secStr.toInt();
  return returnStruct;
}

String timeToStr(timeStruct* obj)
{
  String returnStr = "<li>";
  if(obj->hour < 10)
  {
    returnStr.concat('0');
  }
  returnStr.concat(obj->hour);
  returnStr.concat(':');
  if(obj->minute < 10)
  {
    returnStr.concat('0');
  }
  returnStr.concat(obj->minute);
  returnStr.concat(':');
  if(obj->second < 10)
  {
    returnStr.concat('0');
  }
  returnStr.concat(obj->second);
  returnStr.concat(" UTC on ");
  returnStr.concat(getDayOfWeek(obj));
  returnStr.concat(", ");
  returnStr.concat(getMonth(obj));
  returnStr.concat(' ');
  returnStr.concat(obj->day);
  returnStr.concat(" 20");
  returnStr.concat(obj->year);
  returnStr.concat("</li>");
  return returnStr;
}

String getTime() {
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) {
    // Connection failed, retrying...
  }

  client.print("HEAD / HTTP/1.1\r\n\r\n");
 
  while(!!!client.available()) {
     yield();
  }

  while(client.available()){
    if (client.read() == '\n') {    
      if (client.read() == 'D') {    
        if (client.read() == 'a') {    
          if (client.read() == 't') {    
            if (client.read() == 'e') {    
              if (client.read() == ':') {    
                client.read();
                String theDate = client.readStringUntil('\r');
                client.stop();
                return theDate;
              }
            }
          }
        }
      }
    }
  }
}

void storeTime(timeStruct* obj)
{
  // Evict the last entry
  for(size_t i=0;i<LOG_COUNT;++i)
  {
    if(logNums[i] == (LOG_COUNT-1))
    {
      obj->number = i;
      break;
    }
  }

  int index = 0;
  // Write the new time and update the old time indices
  for(size_t i=0;i<LOG_COUNT;++i)
  {
    if(obj->number == i)
    {
      EEPROM.write(index,0);
      index += 2;
      EEPROM.write(index++,obj->dayOfWeek);
      EEPROM.write(index++,obj->month);
      EEPROM.write(index,obj->day);
      index += 2;
      EEPROM.write(index,obj->year);
      index += 2;
      EEPROM.write(index,obj->hour);
      index += 2;
      EEPROM.write(index,obj->minute);
      index += 2;
      EEPROM.write(index,obj->second);
      index += 2;
    }
    else
    {
      logNums[i] += 1;
      EEPROM.write(index,logNums[i]);
      index += 14;
    }
  }

  // Replace the spot of the recent
  logNums[obj->number] = 0;
  timeVals[obj->number] = *obj;
}

void loadTime()
{
  int index = 0;
  for(size_t i=0;i<LOG_COUNT;++i)
  {
    timeVals[i].number = EEPROM.read(index);
    index += 2;
    timeVals[i].dayOfWeek = EEPROM.read(index++);
    timeVals[i].month = EEPROM.read(index++);
    timeVals[i].day = EEPROM.read(index);
    index += 2;
    timeVals[i].year = EEPROM.read(index);
    index += 2;
    timeVals[i].hour = EEPROM.read(index);
    index += 2;
    timeVals[i].minute = EEPROM.read(index);
    index += 2;
    timeVals[i].second = EEPROM.read(index);
    index += 2;
  }
  
  // Load in log nums array
  int nextVal = 0;
  for(size_t i=0;i<LOG_COUNT;++i)
  {
    for(size_t j=0;j<LOG_COUNT;++j)
    {
      if(timeVals[j].number == nextVal)
      {
        logNums[j] = timeVals[j].number;
        nextVal++;
        break;
      }
    }
  }
}

void convertMonth(timeStruct* obj, String stringIn)
{
  if(stringIn == "Jan")
    obj->month = '0';
  else if(stringIn == "Feb")
    obj->month = '1';
  else if(stringIn == "Mar")
    obj->month = '2';
  else if(stringIn == "Apr")
    obj->month = '3';
  else if(stringIn == "May")
    obj->month = '4';
  else if(stringIn == "Jun")
    obj->month = '5';
  else if(stringIn == "Jul")
    obj->month = '6';
  else if(stringIn == "Aug")
    obj->month = '7';
  else if(stringIn == "Sep")
    obj->month = '8';
  else if(stringIn == "Oct")
    obj->month = '9';
  else if(stringIn == "Nov")
    obj->month = ':';
  else if(stringIn == "Dec")
    obj->month = ';';
  else
    obj->month = '<';
}

void convertDayOfWeek(timeStruct* obj, String stringIn)
{
  if(stringIn == "Sun")
    obj->dayOfWeek = '0';
  else if(stringIn == "Mon")
    obj->dayOfWeek = '1';
  else if(stringIn == "Tue")
    obj->dayOfWeek = '2';
  else if(stringIn == "Wed")
    obj->dayOfWeek = '3';
  else if(stringIn == "Thu")
    obj->dayOfWeek = '4';
  else if(stringIn == "Fri")
    obj->dayOfWeek = '5';
  else if(stringIn == "Sat")
    obj->dayOfWeek = '6';
  else
    obj->dayOfWeek = '7';
}

String getDayOfWeek(timeStruct* obj)
{
  switch(obj->dayOfWeek){
    case '0':
      return "Sunday";
    case '1':
      return "Monday";
    case '2':
      return "Tuesday";
    case '3':
      return "Wednesday";   
    case '4':
      return "Thursday"; 
    case '5':
      return "Friday"; 
    case '6':
      return "Saturday"; 
    default:
      return "?????";        
  }
}

String getMonth(timeStruct* obj)
{
  switch(obj->month){
    case '0':
      return "January";
    case '1':
      return "February";
    case '2':
      return "March";
    case '3':
      return "April";   
    case '4':
      return "May"; 
    case '5':
      return "June"; 
    case '6':
      return "July";
    case '7':
      return "August";
    case '8':
      return "September";   
    case '9':
      return "October"; 
    case ':':
      return "November"; 
    case ';':
      return "December"; 
    default:
      return "?????";     
  }  
}

// Use this function to reset the EEPROM to initial values
void initEEPROM()
{
  int addr = 0;
  for(int i=0;i<LOG_COUNT;++i)
  {
    logNums[i] = i;
    EEPROM.write(addr,i);
    addr += 14;
  }
  for(int i=LOG_COUNT-1;i >= 0 ;--i)
  {
    Serial.print(i);
    saveTimeToEEPROM();
    delay(2000);
  }
  EEPROM.commit();
}
