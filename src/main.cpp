#include <Arduino.h>
#include "sms.h"
#include "sms_alert.h"

int buttonstate = 0;
boolean gps_active = false;
float latitude, longitude, speed_kph, heading, altitude;
char lat[8], lon[8];

void get_gps() //get gps coordinates until it finds and store them
{
  boolean gps_success = getGpsFix(&latitude, &longitude, &speed_kph, &heading, &altitude);

  if (gps_success) //only when finds coordinates
  {
      Serial.print("GPS lat: ");
      Serial.println(latitude, 6);
      dtostrf(latitude, 7, 4, lat);
      Serial.print("GPS long: ");
      Serial.println(longitude, 6);
      dtostrf(longitude, 7, 4, lon);

      gps_active = false;
  }
  else
  {
      Serial.println("Waiting for GPS 3D fix..."); //waiting for 3d fix
  }
}

void setup()
{
  pinMode(7, OUTPUT); //buzzer
  pinMode(8, INPUT); //button

  Serial.begin(9600);

  if (!smsBackendBegin()) //start the GSM module if it's found
  {
    Serial.println(F("Couldn't initialize the GSM module"));
    while (1);
  }
}

void loop()
{
    if(digitalRead(8) == LOW)
    {
        if(buttonstate == 0)
        {
            buttonstate = 1;
        }
        else if(buttonstate == 2)
        {
            buttonstate = 3;
        }
    }

    if(buttonstate == 1)
    {
      sendSmsUser(0); //send sms to user "are you ok?"
      buttonstate = 2;
      gps_active = true;
    }
    if(gps_active && buttonstate == 2)
    {
      get_gps(); //sense gps coordinates and store them
    }
    else if(buttonstate == 3)
    {
      sendSmsEmergency(0, lat, lon); //send emergency sms to user's contacts with coord
      buttonstate = 0;
    }
}
