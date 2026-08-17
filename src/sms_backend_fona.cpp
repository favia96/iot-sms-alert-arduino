#include "sms.h"

#ifdef SMS_BACKEND_FONA

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 10 // 3 on uno --> 10 on mega
#define FONA_RST 4

static SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
static SoftwareSerial *fonaSerial = &fonaSS;
static Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

bool smsBackendBegin()
{
  fonaSerial->begin(9600);
  if (!fona.begin(*fonaSerial)) //start fona if it's found
  {
    Serial.println(F("Couldn't find FONA"));
    return false;
  }

  delay(7000);

  Serial.println(F("Enabling GPS...")); //try to enable gps
  fona.enableGPS(true);
  return true;
}

bool sendSms(const char *number, const char *message)
{
  return fona.sendSMS(const_cast<char *>(number), const_cast<char *>(message));
}

bool getGpsFix(float *lat, float *lon, float *speed_kph, float *heading, float *altitude)
{
  return fona.getGPS(lat, lon, speed_kph, heading, altitude);
}

#endif // SMS_BACKEND_FONA
