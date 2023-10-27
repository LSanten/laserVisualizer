// SimpleRx - the slave or the receiver

// SETUP FOR NRF24
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

//SETUP FOR OLED SCREEN 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)


const byte thisSlaveAddress[5] = { 'R', 'x', 'A', 'A', 'A' };

RF24 radio(CE_PIN, CSN_PIN);


byte dataReceived[8];  // this must match dataToSend in the TX
bool newData = false;

//===========

void setup() {

  Serial.begin(9600);

  //start display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  //start NRF24
  Serial.println("SimpleRx Starting");
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();
}

//=============

void loop() {

  getData();
  showData();
  displayData();
}

//==============

void getData() {
  if (radio.available()) {
    radio.read(&dataReceived, sizeof(dataReceived));
    newData = true;
  }
}

void showData() {
  if (newData == true) {
    Serial.println(dataReceived[0]);
    Serial.println(dataReceived[1]);
    Serial.println(dataReceived[2]);
    newData = false;
  }
}

void displayData() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);

  display.print(F("mode:"));
  display.println(dataReceived[2]);
  display.println(dataReceived[4]);
  display.println(dataReceived[5]);
  display.println(dataReceived[6]);

  display.setCursor(90, 0);
  display.println(dataReceived[1]);

  display.display();

}