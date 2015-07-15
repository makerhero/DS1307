// DS1307_Serial_Easy 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my DS1307-library to 
// quickly send time and date information over a serial link
//
// I assume you know how to connect the DS1307.
// DS1307:  SDA pin   -> Arduino Digital 4
//          SCL pin   -> Arduino Digital 5

#include <DS1307.h>

// Init the DS1307
DS1307 rtc(4, 5);

void setup()
{
   // Initialize the rtc object
  rtc.begin();
  
 // Set the clock to run-mode
  rtc.halt(false);
  
  // Setup Serial connection
  Serial.begin(9600);

  // The following lines can be commented out to use the values already stored in the DS1307
  rtc.setDOW(SUNDAY);        // Set Day-of-Week to SUNDAY
  rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(3, 10, 2010);   // Set the date to October 3th, 2010
}

void loop()
{
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());
  
  // Wait one second before repeating :)
  delay (1000);
}
