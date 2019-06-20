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

int8_t answer;
char aux_string[30];
char phone_number[]="3465759669";   // ********* is the number to call
char sms_text[]="46.068, 11.151";

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
