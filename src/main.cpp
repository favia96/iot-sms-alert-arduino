#include <Arduino.h>

/*
//GPS - GSM
void setup() {
  //Init the driver pins for GSM function
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  //Output GSM Timing
  digitalWrite(5, HIGH);
  delay(1500);
  digitalWrite(5, LOW);
}

void loop() {
  digitalWrite(3, LOW); //enabke GSM TX、RX, HIGH = disable, LOW=enable
  digitalWrite(4, HIGH); //disable GPS TX、RX
}
*/

//GSM
// Product name: GPS/GPRS/GSM Module V3.0
// # Product SKU : TEL0051
// # Version     : 0.1

// # Description:
// # The sketch for driving the gsm mode via the Arduino board

// # Steps:
// #        1. Turn the S1 switch to the Prog(right side)
// #        2. Turn the S2 switch to the Arduino side(left side)
// #        3. Set the UART select switch to middle one.
// #        4. Upload the sketch to the Arduino board
// #        5. Turn the S1 switch to the comm(left side)
// #        6. RST the board

// #        wiki link- http://www.dfrobot.com/wiki/index.php/GPS/GPRS/GSM_Module_V3.0_(SKU:TEL0051)

/*
int8_t answer;
char aux_string[30];
char phone_number[]="3465759669";   // ********* is the number to call
char sms_text[]="Coord: 46.068, 11.151";

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialice the string

    delay(100);

    while( Serial.available() > 0) Serial.read();    // Clean the input buffer

    Serial.println(ATcommand);    // Send the AT command

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(Serial.available() != 0){
            response[x] = Serial.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer) != NULL)
            {
                answer = 1;
            }
        }
    // Waits for the asnwer with time out
    } while((answer == 0) && ((millis() - previous) < timeout));

    return answer;
}

void setup()
{
  //Init the driver pins for GSM function
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  digitalWrite(5,HIGH);//Output GSM Timing
  delay(1500);
  digitalWrite(5,LOW);

  digitalWrite(3,LOW);//Enable the GSM mode
  digitalWrite(4,HIGH);//Disable the GPS mode
  delay(2000);
  Serial.begin(9600); //set the baud rate
  delay(5000);//call ready

  sendATcommand("AT", "OK", 2000);
  Serial.println("Connecting to the network...");
  delay(5000);

  Serial.print("Setting SMS mode...");
  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  Serial.println("Sending SMS");

  sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
  answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
  if (answer == 1)
  {
        Serial.println(sms_text);
        Serial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
            Serial.print("Sent ");
        }
        else
        {
            Serial.print("error ");
        }
    }
    else
    {
        Serial.print("error ");
        Serial.println(answer, DEC);
    }

}

void loop()
{

}
*/

//version 1 --> working

/*
unsigned long time = 0;
unsigned long waiting_time = 20000;
int x = 0;
boolean buttonstate = false;
unsigned int in_frequency = 1000;
unsigned int en_frequency = 2000;
unsigned long duration = 1500;

void setup()
{
  //Init the driver pins for GSM function
  pinMode(7, OUTPUT); //buzzer
  pinMode(8, INPUT); //button

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  digitalWrite(5,HIGH);//Output GSM Timing
  delay(1500);
  digitalWrite(5,LOW);

  digitalWrite(3,LOW);//Enable the GSM mode
  digitalWrite(4,HIGH);//Disable the GPS mode
  delay(2000);
  Serial.begin(9600); //set the baud rate
  delay(5000);//call ready
  delay(5000);
  delay(5000);

  time = millis();
}

void send_sms(){
  tone(7, in_frequency, duration);

  Serial.println("AT"); //Send AT command
  delay(2000);
  Serial.println("AT"); //Send AT command
  delay(2000);

  //Send message
  Serial.println("AT+CMGF=1");
  delay(1000);

  Serial.println("AT+CMGS=\"3465759669\"");//Change the receiver phone number
  delay(1000);

  Serial.println("adad");
  delay(1000);
  Serial.write(26);

  tone(7, en_frequency, duration);
}

void loop()
{
  if(digitalRead(8) == LOW){
    buttonstate = true;
  }
  while(buttonstate == true){
    send_sms();
    buttonstate = false;
  }
}
*/

//version 2
#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 10 // 3 on uno --> 10 on mega
#define FONA_RST 4

#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

//int x = 0;
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

char *names[]={"Federico","Cesar","Marvin"};
char *numbers[]={"+393465759669","+393515059863","+4917657988048"};
char *emergency_numbers[3][3]={
    {"+393515059863", "+4917657988048","+393465759669"},
    {"+393465759669","+4917657988048"},
    {"144","1313","1321"}
};

void flushSerial()
{
  while (Serial.available())
  {
    Serial.read();
  }
}

void send_sms_user(int _i)
{
  delay(2500);
  tone(7, in_frequency, duration);

  sprintf(mex, "Hey %s, are you OK?", names[_i]);
  //Serial.println(mex);
  //delay(1000);
  //Serial.write(26);

  flushSerial();
  fona.sendSMS(numbers[_i], mex);
  delay(2500);
  tone(7, en_frequency, duration);
}

void get_gps()
{
  boolean gps_success = fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude);

  if (gps_success)
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
      Serial.println("Waiting for FONA GPS 3D fix...");
  }
}

void send_sms_emergency(int _i){

  int h;
  for(h = 0; h < 3; h++)
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
      fona.sendSMS(emergency_numbers[_i][h], gps_string_tosend);

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
  if (! fona.begin(*fonaSerial))
  {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }

  delay(7000);

  //try to enable gps
  Serial.println(F("Enabling GPS..."));
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
      send_sms_user(0);
      buttonstate = 2;
      gps_active = true;
    }
    if(gps_active && buttonstate == 2)
    {
      get_gps();
    }
    else if(buttonstate == 3)
    {
      send_sms_emergency(0);
      buttonstate = 0;
    }
}
