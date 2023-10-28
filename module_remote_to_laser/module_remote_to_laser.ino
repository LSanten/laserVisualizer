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

int Button1 = 2;
int Button2 = 3;
int Button3 = 4;
int backwardButtonLastState = 0;
int forwardButtonLastState = 0;
int blueButtonLastState = 0;


byte data[8];

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per 

int setting2BPMMax = 350; //change on both sides
int settingNumber = data[2];


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

    pinMode(Button1, INPUT);
    pinMode(Button2, INPUT);
    pinMode(Button3, INPUT);


    settingNumber = 0;
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

    //Check buttons and change based on buttons
    int backwardButton = digitalRead(Button1);
    int forwardButton = digitalRead(Button2);
    int blueButton = digitalRead(Button3);

    if (backwardButton == 1 and backwardButtonLastState == 0) {
      settingNumber = settingNumber -1;
    }
    else if (forwardButton == 1 and forwardButtonLastState==0) {
      settingNumber = settingNumber +1;
    }

    if (settingNumber <0) {settingNumber = 0;}

    backwardButtonLastState = backwardButton;
    forwardButtonLastState = forwardButton;







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
  if (settingNumber == 1){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);

    display.print(F("<3"));
    display.println(data[2]);
    display.println(data[3]);
    display.println(data[4]);
    display.println(data[5]);

    display.setCursor(90, 0);
    display.println(data[1]);

    display.setCursor(60, 15);
    display.println("PWM");
    display.setCursor(60, 35);
    display.println("Intv");

    display.display();
  }
  else if (settingNumber == 2){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);

    display.println(F("BPM"));
    //display.println(data[2]);
    display.println(data[3]);
    display.println(map(data[4], 0, 255, 0, setting2BPMMax));
    display.println(data[5]);

    display.setCursor(90, 0);
    display.println(data[1]);

    display.setCursor(60, 15);
    display.println("PWM");
    display.setCursor(60, 35);
    display.println("BPM");

    display.display();
  }
  else {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);

    display.print(F("mode:"));
    display.println(data[2]);
    display.println(data[3]);
    display.println(data[4]);
    display.println(data[5]);

    display.setCursor(90, 0);
    display.println(data[1]);

    display.display();
  }

}

//==========

void readPotsToData() {
  data[0] = 1;     //communication group 1= laser
  data[1] = map(analogRead(Potentiometer4),0 , 1023, 0, 255);    //volume
  data[2] = settingNumber;       //setting state
  data[3] = map(analogRead(Potentiometer1), 0, 1023, 0, 255);
  data[4] = map(analogRead(Potentiometer2),0 , 1023, 0, 255);
  data[5] = map(analogRead(Potentiometer3),0 , 1023, 0, 255);
}

