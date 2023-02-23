//Pin reading and writing
int pot1 = A0;
int pot2 = A1;
int switchLaser = 6;


//Global variables
int potValue1 = 0;
int potValue2 = 0;
unsigned long dataTimer = 0;              //used to track how often to communicate data



void setup() {
  Serial.begin(9600);

  pinMode(6, OUTPUT);



}

void loop() {
  // put your main code here, to run repeatedly:
  potValue1 = analogRead(pot1);
  potValue2 = analogRead(pot2);

  Serial.print(potValue1);
  Serial.println(potValue2);

  dataTimer = millis();

  digitalWrite(switchLaser, HIGH);


}
