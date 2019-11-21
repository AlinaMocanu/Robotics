// declare all the joystick pins
const int pinSW = A2; // digital pin connected to switch output
const int pinX = A1; // A0 - analog pin connected to X output
const int pinY = A0; // A1 - analog pin connected to Y output

// declare all the segments pins
const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

const int segSize = 8;

const int noOfDisplays = 4;
const int noOfDigits = 10;

// states of the button press
bool checkSwState = false;
int swState = LOW;
int DPstate = LOW;
int lastSwState = LOW;
int xValue = 0;
int yValue = 0;

bool joyMoved = false;
int digit = 0; /// the digit to be written 
int displayNo = 0; /// the display on which we write the digit
int minThreshold= 400;
int maxThreshold= 600;

int displayValue[noOfDisplays]; /// the values represented on each display

// segments array
int segments[segSize] = {
pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

// digits array, to switch between them easily
int digits[noOfDisplays] = {
pinD1, pinD2, pinD3, pinD4
};

byte digitMatrix[noOfDigits][segSize - 1] = {
// a b c d e f g
{1, 1, 1, 1, 1, 1, 0}, // 0
{0, 1, 1, 0, 0, 0, 0}, // 1
{1, 1, 0, 1, 1, 0, 1}, // 2
{1, 1, 1, 1, 0, 0, 1}, // 3
{0, 1, 1, 0, 0, 1, 1}, // 4
{1, 0, 1, 1, 0, 1, 1}, // 5
{1, 0, 1, 1, 1, 1, 1}, // 6
{1, 1, 1, 0, 0, 0, 0}, // 7
{1, 1, 1, 1, 1, 1, 1}, // 8
{1, 1, 1, 1, 0, 1, 1} // 9
};

void displayNumber(byte digit) {
  for (int i = 0; i < segSize - 1; i++) { 
    digitalWrite(segments[i], digitMatrix[digit][i]);
  }
    
  digitalWrite(segments[segSize-1], LOW);    
}

void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) { 
    digitalWrite(digits[i], HIGH);
  }
  
  digitalWrite(digits[num], LOW);
}

void showDigits() {
  for (int i = 0; i < noOfDisplays; i++) {
    showDigit(i);
    displayNumber(displayValue[i]);
    if (i == displayNo) {
      digitalWrite(segments[segSize-1], HIGH);
    }
    delay (5);
  }
}

/// enables cycling through the 4 digits on the Ox axis
void Xmove() {
  if (xValue > maxThreshold && joyMoved == false) {
    if (displayNo > 0) {
      displayNo--;
    } else {
      displayNo = 3;
    }
    joyMoved = true;
  }
  
  if (xValue < minThreshold && joyMoved == false) {
    if (displayNo < 3) {
      displayNo++;
    } else {
      displayNo = 0;
    }
    joyMoved = true;
  }
  
  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }  
}

/// enables cycling through the digits on Oy axis
void Ymove() {
  digit = displayValue[displayNo];
  if (yValue < minThreshold && joyMoved == false) {
    if (digit > 0) {
      digit--;
    } else {
      digit = 9;
    }
    joyMoved = true;
  }
  
  if (yValue > maxThreshold && joyMoved == false) {
    if (digit < 9) {
      digit++;
    } else {
      digit = 0;
    }
    joyMoved = true;
  }
  
  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }
  
  displayValue[displayNo] = digit; 
}

void setup(){
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  
  for (int i = 0; i < noOfDisplays; i++) {
    pinMode(digits[i], OUTPUT);
  }

  for (int i = 0; i < noOfDisplays; i++) {
    displayValue[i] = 0;
  }
  
  pinMode(pinSW, INPUT_PULLUP);
  
  Serial.begin(9600);
}
void loop(){
  swState = digitalRead(pinSW); 
  if (swState != lastSwState) {
   if (swState == LOW ) {
     checkSwState = !checkSwState;
   }
  }
  lastSwState = swState;

  xValue = analogRead(pinX);
  if (!checkSwState) {
    Xmove();
  }

  yValue = analogRead(pinY);
  if (checkSwState) {
    Ymove();
  }
   
  showDigits();  
}


  
