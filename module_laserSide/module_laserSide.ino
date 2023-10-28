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


// NRF24 things
const byte thisSlaveAddress[5] = { 'R', 'x', 'A', 'A', 'A' };
RF24 radio(CE_PIN, CSN_PIN);
byte data[8];  // this must match dataToSend in the TX

bool newData = false;

//amplifier
#define MAX9744_I2CADDR 0x4B // 0x4B is the default i2c address


//OTHER GLOBAL VARIABLES
int8_t ampVolume = 30; // We'll track the volume level in this variable.
unsigned long millisLoopTime = 0;              //time every loop
unsigned long previousBlinkTime = 0;           //previous millis time

//GLOBAL VAR SETTINGS
int8_t laserPWMState = 255; 
int setting2BPMMax = 350; //change on both sides

int settingNumber = data[2];



#define laserSwitch 6

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

  //laser 
  pinMode(laserSwitch, OUTPUT);
  

  //amplifier
  Wire.beginTransmission(MAX9744_I2CADDR);
  Serial.println("MAX9744 demo");
  Wire.begin();
  
  if (! setvolume(ampVolume)) {
    Serial.println("Failed to set volume, MAX9744 not found!");
    while (1);
  }

  //BEGINNING SETTINGS
  data[0] = 1;
  data[1] = 200;
  data[2] = 1;
  data[3] = 0;
  data[4] = 0;
  data[5] = 0;

  
}

//=============

void loop() {

  millisLoopTime = millis();
  settingNumber = data[2];


  getData();
  //showData();
  displayData();
  setAmpVolume();



  

  if (settingNumber == 1){ //TRADITIONAL PWM & FLICKER
    float interval = map(data[4], 0, 255, 0, 500);
    
    int setPWMvalue = map(data[3], 0, 255, 0, 255);
    blinkEvery(interval, setPWMvalue);
  }
  else if (settingNumber == 2){ //TURN ON OFF BY BPM
    int setPWMvalue = map(data[3], 0, 255, 0, 255);
    float bpm = map(data[4], 0, 255, 0, setting2BPMMax);
    float interval = 60000/bpm;
    Serial.print("bpm");
    Serial.println(bpm);
    blinkEvery(interval, setPWMvalue);
  }
  else if (settingNumber == 0){ //TURN off LASER
    analogWrite(laserSwitch, 0);
  }



  


  


  

  


}

//==============

void getData() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));
    newData = true;
  }
}

void showData() {
  if (newData == true) {
    Serial.println(data[0]);
    Serial.println(data[1]);
    Serial.println(data[2]);
    newData = false;
  }
}

void displayData() {
  Serial.println(settingNumber);
  if (settingNumber == 1){
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



void setAmpVolume(){
  ampVolume = map(data[1], 0, 255, 0, 50); //convert byte to max volume of 63
  setvolume(ampVolume);
}

// Setting the volume is very simple! Just write the 6-bit
// volume to the i2c bus. That's it!
boolean setvolume(int8_t v) {
  // cant be higher than 63 or lower than 0
  if (v > 63) v = 63;
  if (v < 0) v = 0;
  
  //Serial.print("Setting volume to ");
  //Serial.println(v);
  Wire.beginTransmission(MAX9744_I2CADDR);
  Wire.write(v);
  if (Wire.endTransmission() == 0) 
    return true;
  else
    return false;
}


void blinkEvery(float blinkInterval, float setPWMvalue){
  blinkInterval = blinkInterval/2;

  Serial.print("blinkinterval");
  Serial.println(blinkInterval);
  if (millisLoopTime - previousBlinkTime >= blinkInterval and blinkInterval >4) {
    previousBlinkTime = millisLoopTime; 
    if (laserPWMState == 0) {
      laserPWMState = setPWMvalue;
    } else {
      laserPWMState = 0;
    }
  } else if (blinkInterval < 4) {
    laserPWMState = setPWMvalue;
  }
  analogWrite(laserSwitch, laserPWMState);
}
    

