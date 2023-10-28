//Pin reading and writing
int pot1 = A0;
int pot2 = A1;
int laserSwitch = 6;

//Global variables
float potValue1 = 0;
float potValue2 = 0;
unsigned long millisLoopTime = 0;              //time every loop
unsigned long previousBlinkTime = 0;

//Global states
int laserPWMState = 255; 


void setup() {
  Serial.begin(9600);
  pinMode(6, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  potValue1 = analogRead(pot1);
  potValue2 = analogRead(pot2);

  millisLoopTime = millis();

  float interval = potValue1;
  float setPWMvalue = map(potValue2, 0, 1023, 0, 255);
  blinkEvery(interval, setPWMvalue);
  Serial.println(interval);
}
//Functions

void blinkEvery(float blinkInterval, float setPWMvalue){
  blinkInterval = blinkInterval/2;
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
    

