#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 10 // 3 on uno --> 10 on mega
#define FONA_RST 4

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

int buttonstate = 0;
unsigned int in_frequency = 1000;
unsigned int en_frequency = 2000;
unsigned int in_frequency_em = 1500;
unsigned int en_frequency_em = 2500;
unsigned long duration = 1500;
char mex[30];
String gps_string_partial = " has crashed! Coord: ";
String gps_string;
char gps_string_tosend[50];
float latitude, longitude, speed_kph, heading, altitude;
char lat[8], lon[8];
boolean gps_active = false;

char *names[]={"Federico","Cesar","Marvin"}; //users' names

char *numbers[]={"+393465759669","+393515059863","+4917657988048"}; //users' numbers

//emergency numbers (cols) of each user (rows)
char *emergency_numbers[3][3]={
    {"+393515059863","+393465759669"},
    {"+393465759669","+4917657988048"},
    {"144","1313","1321"}
};

void flushSerial() //read serial if available
{
  while (Serial.available())
  {
    Serial.read();
  }
}

void send_sms_user(int _i) //send sms to user "Are you ok?"
{
  delay(2500);
  tone(7, in_frequency, duration);

  sprintf(mex, "Hey %s, are you OK?", names[_i]);
  //Serial.println(mex);
  //delay(1000);
  //Serial.write(26);

  flushSerial();
  fona.sendSMS(numbers[_i], mex); //send sms
  delay(2500);
  tone(7, en_frequency, duration);
}

void get_gps() //get gps coordinates until it finds and store them
{
  boolean gps_success = fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude);

  if (gps_success) //only when finds coordinates
  {
      Serial.print("GPS lat: ");
      Serial.println(latitude, 6);
      dtostrf(latitude, 7, 4, lat);
      //Serial.println(lat);
      Serial.print("GPS long: ");
      Serial.println(longitude, 6);
      dtostrf(longitude, 7, 4, lon);
      //Serial.println(lon);

      gps_active = false;
  }
  else
  {
      Serial.println("Waiting for FONA GPS 3D fix..."); //waiting for 3d fix
  }
}

void send_sms_emergency(int _i) //send coordinates saved to emergency list of the user
{
  int h;
  for(h = 0; h < 2; h++)
  {
      tone(7, in_frequency_em, duration);

      flushSerial();

      //sprintf(gps_string, "Coord: %s , %s ", lat, lon);
      gps_string = names[_i] + gps_string_partial + lat + ", " + lon;
      //Serial.println(gps_string);
      //Serial.println("");
      gps_string.toCharArray(gps_string_tosend, 50);
      //Serial.print("Char: ");
      //Serial.println(gps_string_tosend);
      fona.sendSMS(emergency_numbers[_i][h], gps_string_tosend); //send sms

      tone(7, en_frequency_em, duration);
      delay(7000);
  }
}

void setup()
{
  pinMode(7, OUTPUT); //buzzer
  pinMode(8, INPUT); //button

  while (!Serial);

  Serial.begin(115200);
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) //start fona if it's found
  {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }

  delay(7000);

  Serial.println(F("Enabling GPS...")); //try to enable gps
  fona.enableGPS(true);
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
      send_sms_user(0); //send sms to user "are you ok?"
      buttonstate = 2;
      gps_active = true;
    }
    if(gps_active && buttonstate == 2)
    {
      get_gps(); //sense gps coordinates and store them
    }
    else if(buttonstate == 3)
    {
      send_sms_emergency(0); //send emergency sms to user's contacts with coord
      buttonstate = 0;
    }
}
