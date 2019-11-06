const int potPin0 = A0;
const int potPin1 = A1;
const int potPin2 = A2;


const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;

int potValue0 = 0;
int potValue1 = 0;
int potValue2 = 0;

int ledValue0 = 0;
int ledValue1 = 0;
int ledValue2 = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(potPin0, INPUT);
  pinMode(potPin1, INPUT);
  pinMode(potPin2, INPUT);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  potValue0 = analogRead(potPin0);
  potValue1 = analogRead(potPin1);
  potValue2 = analogRead(potPin2);
  
  ledValue0 = map(potValue0, 0, 1023, 0, 255);
  ledValue1 = map(potValue1, 0, 1023, 0, 255);
  ledValue2 = map(potValue2, 0, 1023, 0, 255);
  
  ledControl(ledValue0, ledValue1, ledValue2);
  
  
}
void ledControl(int ledValue0, int ledValue1, int ledValue2)
{
  analogWrite(redPin, ledValue0);
  analogWrite(greenPin, ledValue1);
  analogWrite(bluePin, ledValue2);
}
