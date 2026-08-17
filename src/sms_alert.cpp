#include <Arduino.h>
#include "sms.h"
#include "sms_alert.h"

static const uint8_t BUZZER_PIN = 7;
static unsigned int in_frequency = 1000;
static unsigned int en_frequency = 2000;
static unsigned int in_frequency_em = 1500;
static unsigned int en_frequency_em = 2500;
static unsigned long duration = 1500;

static char mex[30];
static String gps_string_partial = " has crashed! https://www.google.com/maps?q=";
static String gps_string;
static char gps_string_tosend[50];

static char *names[] = {"User1", "User2", "User3"}; //users' names

static char *phone_number = "+390000000000"; //placeholder phone number

static char *numbers[] = {phone_number, "+392222222222", "+4911111111111"}; //users' numbers

//emergency numbers (cols) of each user (rows)
static char *emergency_numbers[3][3] = {
    {"+392222222222", phone_number},
    {phone_number, "+4911111111111"},
    {"144", "1313", "1321"}};

static void flushSerial() //read serial if available
{
  while (Serial.available())
  {
    Serial.read();
  }
}

void sendSmsUser(int userIndex) //send sms to user "Are you ok?"
{
  delay(2500);
  tone(BUZZER_PIN, in_frequency, duration);

  sprintf(mex, "Hey %s, are you OK?", names[userIndex]);

  flushSerial();
  sendSms(numbers[userIndex], mex); //send sms
  delay(2500);
  tone(BUZZER_PIN, en_frequency, duration);
}

void sendSmsEmergency(int userIndex, const char *lat, const char *lon) //send coordinates saved to emergency list of the user
{
  for (int h = 0; h < 2; h++)
  {
    tone(BUZZER_PIN, in_frequency_em, duration);

    flushSerial();

    gps_string = names[userIndex] + gps_string_partial + lat + ", " + lon;
    gps_string.toCharArray(gps_string_tosend, 50);
    sendSms(emergency_numbers[userIndex][h], gps_string_tosend); //send sms

    tone(BUZZER_PIN, en_frequency_em, duration);
    delay(7000);
  }
}
