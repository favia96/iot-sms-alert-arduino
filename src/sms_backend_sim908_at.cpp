#include "sms.h"

#ifdef SMS_BACKEND_SIM908_AT

#include <Arduino.h>
#include <string.h>

// Raw AT-command driver for the DFRobot GPS/GPRS/GSM Module V3.0 (SIM908),
// ported from the sendSMS prototype. Talks to the module directly over
// Serial instead of a vendor library.
//
// Steps to use the shield:
//        1. Turn the S1 switch to the Prog (right side)
//        2. Turn the S2 switch to the Arduino side (left side)
//        3. Set the UART select switch to the middle position
//        4. Upload the sketch to the Arduino board
//        5. Turn the S1 switch to Comm (left side)
//        6. Reset the board
//
// wiki: http://www.dfrobot.com/wiki/index.php/GPS/GPRS/GSM_Module_V3.0_(SKU:TEL0051)

static int8_t sendATcommand(const char *ATcommand, const char *expected_answer, unsigned int timeout)
{
    uint8_t x = 0, answer = 0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100); // Initialize the string

    delay(100);

    while (Serial.available() > 0) Serial.read(); // Clean the input buffer

    Serial.println(ATcommand); // Send the AT command

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do
    {
        // if there are data in the UART input buffer, reads it and checks for the answer
        if (Serial.available() != 0)
        {
            response[x] = Serial.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer) != NULL)
            {
                answer = 1;
            }
        }
        // Waits for the answer with time out
    } while ((answer == 0) && ((millis() - previous) < timeout));

    return answer;
}

bool smsBackendBegin()
{
  //Init the driver pins for GSM function
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  digitalWrite(5, HIGH); //Output GSM Timing
  delay(1500);
  digitalWrite(5, LOW);

  digitalWrite(3, LOW);  //Enable the GSM mode
  digitalWrite(4, HIGH); //Disable the GPS mode
  delay(2000);

  delay(5000); //call ready

  sendATcommand("AT", "OK", 2000);
  Serial.println("Connecting to the network...");
  delay(5000);

  Serial.print("Setting SMS mode...");
  sendATcommand("AT+CMGF=1", "OK", 1000); // sets the SMS mode to text
  return true;
}

bool sendSms(const char *number, const char *message)
{
  char aux_string[30];

  sprintf(aux_string, "AT+CMGS=\"%s\"", number);
  if (sendATcommand(aux_string, ">", 2000) != 1) // send the SMS number
  {
    return false;
  }

  Serial.println(message);
  Serial.write(0x1A);
  return sendATcommand("", "OK", 20000) == 1;
}

bool getGpsFix(float *lat, float *lon, float *speed_kph, float *heading, float *altitude)
{
  // Not implemented for this backend yet - the SIM908's GPS is driven via a
  // separate set of AT commands (see iot-gps-arduino's gps_sensor.h for a
  // reference implementation) that this project doesn't include.
  (void)lat;
  (void)lon;
  (void)speed_kph;
  (void)heading;
  (void)altitude;
  return false;
}

#endif // SMS_BACKEND_SIM908_AT
