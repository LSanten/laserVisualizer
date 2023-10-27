// SimpleTx - the transmitter
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define CE_PIN   9
#define CSN_PIN 10
const byte slaveAddress[5] = {'R','x','A','A','A'};
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
char txNum = '0';

//OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)




#define Potentiometer1 A0
#define Potentiometer2 A1
#define Potentiometer3 A2
#define Potentiometer4 A3


byte data[8];

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second


void setup() {

    Serial.begin(9500);

    Serial.println("SimpleTx Starting");

    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.setRetries(3,5); // delay, count
    radio.openWritingPipe(slaveAddress);

    //start display
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
       Serial.println(F("SSD1306 allocation failed"));
       for(;;);
     } 


    delay(100);
    display.clearDisplay();
    display.setTextSize(2);     
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    // Display static text
    display.println("starting");
    display.display(); 
    delay(200);
    display.println("remote");
    display.display(); 
    delay(200);
    display.println("<3");
    display.display(); 

    pinMode(Potentiometer1,INPUT);
    pinMode(Potentiometer2, INPUT);
}

//====================

void loop() {
    currentMillis = millis();

    readPotsToData();

    //Serial.println(data[1]);
    //Serial.println(data[2]);

    send();
    //delay(50);

    displayData();





}

//====================
// FUNCTIONS BELOW 
//====================

void send() {

    bool rslt;

    rslt = radio.write( &data, sizeof(data) );
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    Serial.print("Data Sent ");
    //Serial.print(dataToSend);
    if (rslt) {
        Serial.println("  Acknowledge received");
    }
    else {
        Serial.println("  Tx failed");
    }
}

//================

void displayData() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);

  display.print(F("mode:"));
  display.println(data[2]);
  display.println(data[4]);
  display.println(data[5]);
  display.println(data[6]);

  display.setCursor(90, 0);
  display.println(data[1]);

  display.display();

}

//==========

void readPotsToData() {
  data[0] = 1;     //communication group 1= laser
  data[1] = map(analogRead(Potentiometer4),0 , 1023, 0, 255);    //volume
  data[2] = 1;       //setting state
  data[4] = map(analogRead(Potentiometer1), 0, 1023, 0, 255);
  Serial.println(Potentiometer3);
  Serial.println(data[6]);
  data[5] = map(analogRead(Potentiometer2),0 , 1023, 0, 255);
  data[6] = map(analogRead(Potentiometer3),0 , 1023, 0, 255);
}

