#include <LiquidCrystal.h>
#include <EEPROM.h>

/// lcd pins
const int RS = 12;
const int enable = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

/// joystick pins
const int pinSW = 10; // digital pin connected to switch output
const int pinY = A0; // A1 - analog pin connected to Y output
const int pinX = A1; // A0 - analog pin connected to X output

/// states of the button press
int swState;
int lastSwState = HIGH;
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50;

int switchValue;
int yValue = 0;
int xValue = 0;

bool joyMoved = false;
int minThreshold= 400;
int maxThreshold= 600;

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value = 0;

char incomingByte = 0; // for incoming serial data
String inputString = "";
String lastInputString = "";
bool stringComplete = false;

/// boolean variables which tell us where we are located
bool locatedInSettings = false;
bool locatedInStart = false;
bool locatedInMenuPage = true;
int cursorPosition = 0; /// to scroll through the items


const int noOfLives = 3;
int startingLevelValue = 0;
int levelValue = 0;
int score = 0;
int highscore = 0;

const int gameDuration = 10000; 
unsigned long startingTime = 0;

const int incrementLevelTime = 5000;
unsigned long lastLevelIncrementTime = 0;


const int noOfSubItems = 3;
const int noOfMenItems = 3;

String menuItems[noOfMenItems] = {"Start", "Settings", "Highscore"};
String submenuItems[noOfSubItems] = {"Lives", "Level", "Score"};

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  pinMode(pinSW, INPUT_PULLUP); // activate pull-up resistor on the push-button pin

  // set the cursor to column 0, line 0
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">");
  printMenu();

  Serial.begin(9600);
}
void loop() {
  // if we are in the main menu page, we should be able to scroll through the items 
  if( locatedInMenuPage == true) {
    Ymove();
    moveCursor();
  } 
  
  enterAndExitSubmenu(); // selecting an option while we are in the main page, but also use it to exit submenu

  // in the "Settings" submenu, the available options are entering a name and selecting the starting level
  if (locatedInSettings) {
     setStartingLevelValue();
     enterName();
  }

  // once the game starts, the starting level will be incremented by 1 each 5 seconds. The game ends after 10 seconds
  if (locatedInStart) {
     incrementLevel();
     endGame();
  }
}

// function used to move on the Ox axis
void Xmove() {
  xValue = analogRead(pinX);
  if (xValue < minThreshold && joyMoved == false) {
    if (startingLevelValue > 0) {
        startingLevelValue--;
    }
    
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    startingLevelValue++;
    joyMoved = true;
  }
  
  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  } 
}

// function used to move on the Oy axis
void Ymove() {
  yValue = analogRead(pinY);
  if (yValue < minThreshold && joyMoved == false) {
    if (cursorPosition > 0) {
        cursorPosition--;
    } else {
        cursorPosition = noOfMenItems - 1;
    }
    joyMoved = true;
  }

  if (yValue > maxThreshold && joyMoved == false) {
    if (cursorPosition < noOfMenItems - 1 ) {
        cursorPosition++;
    } else {
        cursorPosition = 0;
    }
    joyMoved = true;
  }
  
  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }
}

void moveCursor() {
  // only one arrow can point at a submenu at a time, so make sure to eliminate duplicates
  switch(cursorPosition){
    case 0: {
              lcd.setCursor(menuItems[0].length() + 2, 0);
              lcd.print(" ");
              lcd.setCursor(0, 1);
              lcd.print(" ");
              
              lcd.setCursor(0, 0);
              lcd.print(">");
              
              break;
    }
    case 1: {
              lcd.setCursor(0, 0);
              lcd.print(" ");
              lcd.setCursor(0, 1);
              lcd.print(" ");

            
              lcd.setCursor(menuItems[0].length() + 2, 0);
              lcd.print(">");
              
              break;
    }
    case 2: {
              lcd.setCursor(0, 0);
              lcd.print(" ");
              lcd.setCursor(menuItems[0].length() + 2, 0);
              lcd.print(" ");
              
              lcd.setCursor(0, 1);
              lcd.print(">");
              
              break;  
   }
  }
}

void printMenu() {
  lcd.clear();
  for (int i = 0; i < noOfMenItems; i++) {
    switch(i) {
      case 0: {
                lcd.setCursor(1, 0);
                lcd.print(menuItems[i]);
                
                break;
      }
      case 1: {
                lcd.setCursor(menuItems[i - 1].length() + 3, 0);
                lcd.print(menuItems[i]);
                
                break;
      }
      case 2: {
                lcd.setCursor(1, 1);
                lcd.print(menuItems[i]);
                break;
              }
      }
        
    }

    // by printing the menu, automatically we have exited one of the two submenus
    locatedInSettings = false;
    locatedInStart = false;
}

void printSubmenu() {
  lcd.clear();

  // print the submenu pointed by the arrow
  switch(cursorPosition) {
    case 0: { 
               for (int i = 0; i < noOfSubItems; i++) {
                switch(i) {
                  case 0: {
                             lcd.setCursor(1, 0);
                             lcd.print(submenuItems[i]);
                             lcd.print(":");
                             lcd.print(noOfLives);
                             
                             break;
                  }
                  case 1: {
                             lcd.setCursor(submenuItems[i - 1].length() + 4, 0);
                             lcd.print(submenuItems[i]);
                             lcd.print(":");
                             lcd.print(levelValue);
                             
                             break;
                  }
                  case 2: {
                             score = 0; // when level is reseted, the score resets a few seconds later, so reinitialize score fo a faster result
                             lcd.setCursor(1, 1);
                             lcd.print(submenuItems[i]);
                             lcd.print(":");
                             lcd.print(score);
                             
                             break;
                  }
                 }
               }

              // memorize the starting time of the game
              startingTime = millis();
              lastLevelIncrementTime = millis();
              locatedInStart = true;
            
              break;
    }
    case 1: {
              lcd.setCursor(1, 0);
              lcd.print("Start level:");
              lcd.setCursor(1, 1);
              lcd.print("Player:");
              lcd.setCursor(9, 1);
              lcd.print(lastInputString);
              locatedInSettings = true;
              
              break;
    }
    case 2: {
              // print the  saved highscore
              lcd.setCursor(1, 0);
              address = 0;
              value = EEPROM.read(address);
              lcd.print(value);
         
              break;
    }
  }
    
}

void enterAndExitSubmenu() {
  int reading = digitalRead(pinSW);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastSwState) { 
    // reset the debouncing timer
    lastDebounceTime = millis(); 
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the
    // debounce delay, so take it as the actual current state:

    
    if (reading != swState) {
      swState = reading;
      if (swState == LOW){ 
        /// if the button is pressed and you are located in the menu, then you must enter one of the submenus and vice versa
        if (locatedInMenuPage == true) {
           printSubmenu(); 
        }
        else if (locatedInMenuPage == false) {
           printMenu();
        }
        locatedInMenuPage = !locatedInMenuPage;
      }
    }
  }
    
  lastSwState = reading;
}

void setStartingLevelValue() {
  // choose a starting level and then print it to the lcd
  Xmove();   
  lcd.setCursor(14 ,0);
  lcd.print(startingLevelValue);
  levelValue = startingLevelValue;
}

void endGame() {
  if (millis() - startingTime > gameDuration) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Congrats! Press");
    lcd.setCursor(0, 1);
    lcd.print("button to exit");
    locatedInStart = false;
      
    // update the highscore and save it
    if (score > highscore){
      highscore = score;
      saveToEEPROM();
    }
  }
}

void incrementLevel() {
  if (millis() - lastLevelIncrementTime > incrementLevelTime) {
    lastLevelIncrementTime = millis(); // memorize last increment so to know when 5 seconds have passed since incrementing the level
    levelValue++;
       
    lcd.setCursor(submenuItems[0].length() + submenuItems[1].length() +  5, 0);
    lcd.print("");
    lcd.print(levelValue);

    // update the score
    score = 3 * levelValue;
    lcd.setCursor(submenuItems[2].length() + 2, 1);
    lcd.print("");
    lcd.print(score);    
  }
}

void enterName() {
  if (Serial.available() > 0) {
    incomingByte = (char)Serial.read();
    
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (incomingByte == '\n') {
      stringComplete = true;
      lcd.setCursor(8, 1);
      lcd.print(inputString);
      
      // make sure to "clean up" when inserting a new name for player that has a smaller length than the last one
      for (int j = 1; j <= 8 - inputString.length(); j++) 
        lcd.print(" ");
        
      lastInputString = inputString;
      inputString = "";
    } else {
      inputString += incomingByte;
     }
  }
}

void saveToEEPROM() {
  address = 0;
  EEPROM.write(address, highscore);
  address = address + 1;
}
