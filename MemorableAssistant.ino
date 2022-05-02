#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include "UbidotsMicroESP8266.h"
#define ID "5c054d74c03f973fb97a4b4d" // Put here your Ubidots variable ID ID FOR variable
#define ID2 "5c054d60c03f973fb97a4b3e" // Put here your Ubidots variable ID MY PROJECT
#define TOKEN "A1E-yGG36PY2tQewFold78juAves7Z1mkj" // Put here your Ubidots TOKEN
#define WIFISSID "AndroidAP1"
#define PASSWORD "siricandoit2789"
Ubidots client(TOKEN);
// OLED display TWI address
#define OLED_ADDR   0x3C
TinyGPSPlus gps;  // The TinyGPS++ object

SoftwareSerial ss(13, 12); // The serial connection to the GPS device


Adafruit_SSD1306 display(-1);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
int PulseSensorPurplePin = 0;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
int LED15 = 15;   //  The on-board Arduion LED


int Signal;                // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 550;            // Determine which Signal to "count as a beat", and which to ignore.
const int buttonPin = 15;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status

float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str;
int pm;

void pulse()
{
  Signal = analogRead(PulseSensorPurplePin);  // Read the PulseSensor's value.
  // Assign this value to the "Signal" variable.

  Serial.println(Signal);
  delay(2000);// Send the Signal value to Serial Plotter.
  display.setCursor(94, 0);
  display.setTextSize(0.005);
  display.print("Pulse");
  display.setCursor(94, 8);
  display.println(Signal);
     client.add("Pulse",Signal );
        client.sendAll(true);



  if (Signal > Threshold) {                        // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
    digitalWrite(LED15, HIGH);
  } else {
    digitalWrite(LED15, LOW);               //  Else, the sigal must be below "550", so "turn-off" this LED.
  }


  delay(10);


}
void button() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  delay(500);
  Serial.println(buttonState);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
  client.add("Need Help",buttonState );
        client.sendAll(true);

  } else {
  client.add("No Problem",buttonState );
        client.sendAll(true);

  }
}
void setup()
{ 
    pinMode(buttonPin, OUTPUT);
 client.wifiConnection(WIFISSID, PASSWORD);
  // initialize and clear display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();
  // display a pixel in each corner of the screen
  display.drawPixel(0, 0, WHITE);

  display.drawPixel(0, 31, WHITE);


  // display a line of text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(27, 30);
  display.print(" ");

  // update display with all of the above graphics
  display.display();
  Serial.begin(115200);
  ss.begin(9600);
  

}

void loop()
{

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
    {
   

      if (gps.date.isValid())
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
          date_str = '0';
        date_str += String(date);

        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month);

        date_str += " / ";

        if (year < 10)
          date_str += '0';
        date_str += String(year);
      }

      if (gps.time.isValid())
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();

        minute = (minute + 30);
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23)
          hour = hour - 24;

        if (hour >= 12)
          pm = 1;
        else
          pm = 0;

        hour = hour % 12;

        if (hour < 10)
          time_str = '0';
        time_str += String(hour);

        time_str += " : ";

        if (minute < 10)
          time_str += '0';
        time_str += String(minute);

        time_str += " : ";

        if (second < 10)
          time_str += '0';
        time_str += String(second);

        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";

      }

    }
   display.display();
    if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    pulse();
   
  }
 
   if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        Serial.println(lat_str);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
         Serial.println(lng_str);
         display.setCursor(0, 2);
         display.print("Lat ");
         display.println(gps.location.lat(), 4);
         display.print("Long ");
         display.println(gps.location.lng(), 4);
        client.add("Latitude", latitude);
        client.add("Longitude",longitude );
        client.sendAll(true);
         pulse();
         button();
        
        
      }

 


}
