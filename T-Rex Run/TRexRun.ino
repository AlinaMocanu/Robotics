#include <MaxMatrix.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
/// lcd pins
const int RS = 7;
const int enable = 6;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);
MaxMatrix m(12, 10, 11, 1);

// declare all the joystick pins
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output
const int pinSW = 9; // pinSW - digital pin connected to the Sw output

// push button and buzzer pin
const int pushButton = 13;
const int passiveBuzzer = 8;

int noteDuration = 1000 / 8;
int pauseBetweenNotes = noteDuration * 0.2;
int toneIncrease = 700;

int swState = HIGH;
int lastSwState = HIGH;
int switchValue = 0;
int xValue = 0;
int yValue = 0;

bool xJoyMoved = false;
bool yJoyMoved = false;
bool joyMovedStart = false;
bool levelJoyMoved = false;
bool infoJoyMoved = false;

bool jumping = false;

const int minThreshold = 400; 
const int maxThreshold = 600;

// place the dot on the matrix
int row = 1;
int col = 2;

unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50;

unsigned long lastJump = 0; 
unsigned long jumpingInterval = 500;
int jumpLength = 3;


unsigned long lastIncreaseInSpeed = 0;
unsigned long speedIncreaseInterval = 10000;

unsigned long lastShift = 0;
unsigned long shiftSpeed = 300; 

int randIndex = 0; // random index of shape to be displayed
int address = 0; // offset for EEPROM memory
int nameLength = ""; // length of player's name
int highScoreValue = 0;

int score = 0;
int highScore = 0;
int noOfLives = 3;
int level = 1;
int startingLevelValue = 1; // used to increment level
bool gameState = false;

// variables used to indicate the page that is being printed on the lcd 
bool locatedInSettings = false;
bool locatedInStart = false;
bool locatedInMenuPage = true;
bool locatedInHighScore = false;
bool locatedInInfo = false;

// variables used to move the cursor in menu and submenus
int cursorPositionStart = 0;
int cursorPositionMenu = 0;
int cursorPositionInfo = 0;
int lastCursorPositionInfo = 0;

// variables used to read, store and print player's name
char incomingByte; // for incoming serial data
String inputString = "";
String lastInputString = "";
String highScorePlayer = "";

// shapes that will be continuously shifted on the matrix
const int noOfBytes = 11;
byte shapesLevel1[][noOfBytes] = {
                                  {8, 8, B0000001, B0000001, B0000111, B0000001, B0000001, B0000001, B0000001, B0000001}, // shape 1
                                  {8, 8, B0000001, B0000001, B0000011, B0000001, B0000001, B0000001, B0000001, B0000001}, // shape 2
                                  {8, 8, B0000011, B0000001, B0000001, B0000001, B0000001, B0000001, B0000001, B0000001}, // shape 3
                                  {8, 8, B0000001, B0000011, B0000001, B0000001, B0000001, B0000001, B0000011, B0000001}, // shape 4
                                  {8, 8, B0000001, B0000111, B0000001, B0000001, B0000001, B0000001, B0000001, B0000001}, // shape 5
                                  {8, 8, B0000001, B0000000, B0000001, B0000001, B0000001, B0000001, B0000001, B0000001}, // shape 6
                                 };

byte shapesLevel2[][noOfBytes] = {
                                  {8, 8, B00000001, B00001111, B00001111, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 1
                                  {8, 8, B00000001, B11111101, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 2
                                  {8, 8, B00000011, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 3
                                  {8, 8, B00000001, B00000001, B00000011, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 4
                                  {8, 8, B00000001, B00000011, B00000111, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 5
                                  {8, 8, B00000001, B11100011, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 6
                                 };
byte shapesLevel3[][noOfBytes] = {
                                  {8, 8, B11001111, B00000001, B11001111, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 1
                                  {8, 8, B11000000, B00000000, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 2
                                  {8, 8, B00000111, B00000001, B00001111, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 3
                                  {8, 8, B00000001, B00000000, B00000000, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 4
                                  {8, 8, B00001111, B00001111, B11000001, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 5
                                  {8, 8, B00000001, B11100111, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001}, // shape 6
                                 };
                  
// used for intro countdown 
byte digits[][noOfBytes] = {   
                            {8, 8, B0000000, B0000000, B0010000, B0100001, B1111111, B0000001, B0000000, B0000000}, // 1
                            {8, 8, B0000000, B0000000, B1001111, B1001001, B1001001, B1111001, B0000000, B0000000}, // 2
                            {8, 8, B0000000, B0000000, B1001001, B1001001, B1001001, B1111111, B0000000, B0000000}, // 3
                          };
                  
byte sadFace[noOfBytes] = {8, 8, B0111100, B01000010, B10100101, B10001001, B10001001, B10100101, B01000010, B0111100 };

const int noOfMenItems = 4;

String menuItems[noOfMenItems] = {"Start", "Settings", "Highscore", "Info"};

void setup()
{
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pushButton, INPUT_PULLUP);

  //set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.clear();
  printMenu();
  
  lcd.setCursor(0, 0);
  lcd.print(">");
  
  m.init();
  m.setIntensity(4); //change brightness

  Serial.begin(9600);
  EEPROM.write(0, 0);
}   

void loop()       
{    
  if (gameState) {
    printGameDetails();

    // display the ground as the shapes haven't been shifted yet
    for (int i = 0; i <= 7; i++)
        m.setDot(i, 0, 1);
        
    randIndex = random(6);

    // increase speed each 10 seconds
    if (millis() - lastIncreaseInSpeed > speedIncreaseInterval) {
      shiftSpeed -= 50; 
      jumpingInterval -= 50;

      if (shiftSpeed == 100) {
        if (level < 3) {
          level++;
          shiftSpeed = 300;
          jumpingInterval = 500;
         }
       }
       
      if (level == 1)
         printShapeWithShift(shapesLevel1[randIndex], shiftSpeed);
      else 
      if (level == 2)
         printShapeWithShift(shapesLevel2[randIndex], shiftSpeed);
      else 
      if (level == 3)
         printShapeWithShift(shapesLevel3[randIndex], shiftSpeed);
         
      lastIncreaseInSpeed = millis();
    } else {
      if (level == 1) 
         printShapeWithShift(shapesLevel1[randIndex], shiftSpeed);
      else
      if (level == 2) 
         printShapeWithShift(shapesLevel2[randIndex], shiftSpeed);
      else
      if (level == 3) 
         printShapeWithShift(shapesLevel3[randIndex], shiftSpeed);  
    } 
  } else if (locatedInMenuPage) { // if located in the menu page, you should be able to scroll through the otpions and select one
    browseMenu();
    enterAndExitSubmenu();
  } else if (locatedInHighScore) { 
    enterAndExitSubmenu();
  } else if (locatedInSettings) {
    enterName();
    setStartingLevelValue();
    enterAndExitSubmenu();
  } else if (locatedInInfo) { // in Info section you should be able to browse through the pages
    browseInfo(); 
    enterAndExitSubmenu();
  } else if (locatedInStart) {
    browseStart();
    enterAndExitSubmenu();
  }  
}

// utility function used to scroll through menu items
void YmoveMenu() {
  yValue = analogRead(pinY);
  if (yValue < minThreshold && yJoyMoved == false) {
    if (cursorPositionMenu > 0) 
      cursorPositionMenu--;
    else 
      cursorPositionMenu = noOfMenItems - 1;
    
    yJoyMoved = true;
  }

  if (yValue > maxThreshold && yJoyMoved == false) {
    if (cursorPositionMenu < noOfMenItems - 1 ) 
        cursorPositionMenu++;
    else 
        cursorPositionMenu = 0;
    yJoyMoved = true;
  }
  
  if (yValue >= minThreshold && yValue <= maxThreshold) {
    yJoyMoved = false;
  }
}

// utility function used to scroll through start items
void YmoveStart() {
  yValue = analogRead(pinY);
  if (yValue < minThreshold && yJoyMoved == false) {
    if (cursorPositionStart > 0) 
       cursorPositionStart--;
    else 
       cursorPositionStart = 1;
    
    yJoyMoved = true;
  }

  if (yValue > maxThreshold && yJoyMoved == false) {
    if (cursorPositionStart < 1 ) 
      cursorPositionStart++;
    else 
      cursorPositionStart = 0;
    yJoyMoved = true;
  }
  
  if (yValue >= minThreshold && yValue <= maxThreshold) {
    yJoyMoved = false;
  }
}

void browseMenu() {
  YmoveMenu();
  switch (cursorPositionMenu){
    case 0: {
              lcd.setCursor(menuItems[0].length() + 2, 0);
              lcd.print(" ");
              lcd.setCursor(0, 1);
              lcd.print(" ");
              lcd.setCursor(menuItems[2].length() + 1, 1);
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
              lcd.setCursor(menuItems[2].length() + 1, 1);
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
              lcd.setCursor(menuItems[2].length() + 1, 1);
              lcd.print(" ");
              
              lcd.setCursor(0, 1);
              lcd.print(">");
              
              break;  
   }
   case 3: {
              lcd.setCursor(0, 0);
              lcd.print(" ");
              lcd.setCursor(menuItems[0].length() + 2, 0);
              lcd.print(" ");
              lcd.setCursor(0, 1);
              lcd.print(" ");
              
              lcd.setCursor(10, 1);
              lcd.print(">");
              
              break;  
   }
  }
}

void browseStart() {
  YmoveStart();
  switch (cursorPositionStart){
    case 0: {
              lcd.setCursor(0, 1);
              lcd.print(" ");
              
              lcd.setCursor(10, 0);
              lcd.print(">");
              
              break;
    }
    case 1: {
              lcd.setCursor(10, 0);
              lcd.print(" ");

              lcd.setCursor(0, 1);
              lcd.print(">");
              
              break;
    }
  }
}

void gameIntro() {
  toneIncrease = 700;
  noteDuration = 100;
  
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Run, T-REX, Run!");
  
  for (int i = 2 ; i >= 0; i--){
    m.writeSprite(0, 0, digits[i]);
    tone(passiveBuzzer, toneIncrease, noteDuration);
    toneIncrease += 10;
    noteDuration += 25; 
    delay(1000);
  }
  m.clear(); 
}

// reset variables for a new game
void resetGame() {
  score = 0;
  level = startingLevelValue;
  noOfLives = 3; 
  shiftSpeed = 300; 
  jumpingInterval = 500;
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
           // if the "start" option has been selected 
           if (cursorPositionMenu == 0) {
             locatedInMenuPage = false;
             locatedInStart = true;
             gameState = true;
             
             printGameDetails();
             gameIntro();
           } else if (cursorPositionMenu == 2) { // if the High Score section is selected
             printHighScore();
             locatedInHighScore = true;
             locatedInMenuPage = false;
           } else if (cursorPositionMenu == 1) { // if the Settings section is selected
             lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print("Player");
             
             lcd.setCursor(0, 1);
             lcd.print("Level");
             lcd.setCursor(6, 1);
             lcd.print(level);
             
             locatedInSettings = true;
             locatedInMenuPage = false;
          } else if (cursorPositionMenu == 3) { // if the Info section is selected
             lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print("T-Rex Run!");
             
             lcd.setCursor(0, 1);
             lcd.print("by Alina Mocanu");
             
             locatedInInfo = true;
             locatedInMenuPage = false;
             cursorPositionInfo = 0;
          }
        } else if (locatedInStart) {
          // if the "Menu" option has been selected
          if (cursorPositionStart == 0){
            printMenu(); 
            locatedInStart = false;
            locatedInMenuPage = true;
          }
          else if (cursorPositionStart == 1) {  // if the "restart" option has been selected
              gameState = true;
              resetGame();
              printGameDetails();
              gameIntro();
          }         
        } else if (locatedInHighScore) {
          printMenu();
          locatedInMenuPage = true;
          locatedInHighScore = false; 
        } else if (locatedInSettings) {
          printMenu();
          locatedInMenuPage = true;
          locatedInSettings = false; 
        } else if (locatedInInfo) {
          printMenu();
          locatedInMenuPage = true;
          locatedInInfo = false; 
       }
    }
   } 
}
  
  
  if (locatedInInfo && cursorPositionInfo != lastCursorPositionInfo) {
    if (cursorPositionInfo == 2){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Robotics/tree/ma");
      
      lcd.setCursor(0, 1);
      lcd.print("ster/T-Rex%20Run");
    } else if (cursorPositionInfo == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("https://github");
      
      lcd.setCursor(0, 1);
      lcd.print(".com/AlinaMocanu/");
    } else if (cursorPositionInfo == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("T-Rex Run!");
      
      lcd.setCursor(0, 1);
      lcd.print("by Alina Mocanu");
    }
    
    lastCursorPositionInfo = cursorPositionInfo;
  }
  
  lastSwState = reading;
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
      case 3: {
                lcd.setCursor(11, 1);
                lcd.print(menuItems[i]);
                break;
              }
      }
        
    }

  // by printing the menu, automatically we have exited one of the two submenus
  locatedInSettings = false;
  locatedInStart = false;
}

void Jump() {
  int reading = digitalRead(pushButton);
  
  // check to see if you just pressed the button
  // (i.e. the input went from HIGH to LOW), and you've waited long enough
  // since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  if (reading != lastSwState) {
  // reset the debouncing timer
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
  // whatever the reading is at, it's been there for longer than the debounce delay, so take it as the actual current state:
  
    // if the button state has changed:
    if (reading != swState) {
      swState = reading;
       
      if (swState == LOW && jumping == false) { // jumping should be false so the dot doesn't do a double jump
        tone(passiveBuzzer, 700, 100);
        
        lastJump = millis(); 
        jumping = true;
        
        m.setDot(col, row, 0);
        row += jumpLength;
        m.setDot(col, row, 1);
      }
    }
  }
  
  // save the reading. Next time through the loop, it'll be the lastSwState:
  lastSwState = reading; 
}

void browseInfo() {
  xValue = analogRead(pinX);
  if (xValue < minThreshold && infoJoyMoved == false) {
    if (cursorPositionInfo > 0) 
      cursorPositionInfo--; 
    
    infoJoyMoved = true;
  }

  if (xValue > maxThreshold && infoJoyMoved == false) {
    if (cursorPositionInfo < 2)
      cursorPositionInfo++;
       
    infoJoyMoved = true;
  }
  
  if (xValue >= minThreshold && xValue <= maxThreshold) {
    infoJoyMoved = false;
  }
}

void browseLevel() {
  xValue = analogRead(pinX);
  if (xValue < minThreshold && levelJoyMoved == false) {
    if (startingLevelValue > 1) 
      startingLevelValue--;
     
    levelJoyMoved = true;
  }

  if (xValue > maxThreshold && levelJoyMoved == false) {
    if (startingLevelValue < 3)
      startingLevelValue++;
      
    levelJoyMoved = true;
  }
  
  if (xValue >= minThreshold && xValue <= maxThreshold) {
    levelJoyMoved = false;
  } 
}

void setStartingLevelValue() {
  // choose a starting level and then print it to the lcd
  browseLevel();   
  level = startingLevelValue;
  lcd.setCursor(6 ,1);
  lcd.print(level);
}

void failSound() {
  noteDuration = 1000/ 8;
    
   // Determine the number of siren cycles to play
  for (int numOfLoops = 0; numOfLoops < 4; numOfLoops++) {

    // play low to high
    for (int i = 25; i < 30; i++) {
       tone(passiveBuzzer, 20 * i, noteDuration);
       delay(pauseBetweenNotes);
    }

    // play high to low
    for (int i = 30; i >= 25; i--) {
       tone(passiveBuzzer, 20 * i, noteDuration);
       delay(pauseBetweenNotes);
    }
  }
  
  if (noOfLives == 0) {
    delay(500);
    tone(passiveBuzzer, 500, 200);  
    
    delay(200);
    tone(passiveBuzzer, 1200, 200);
    
    delay(200);
    tone(passiveBuzzer, 300, 200);
    
    delay(200);
    tone(passiveBuzzer, 1000, 200);
    
    delay(200);
    tone(passiveBuzzer, 400, 200);
    
    delay(200);
    tone(passiveBuzzer, 1100, 200);
    delay(200);
  }
}

 // update the high score and print on the lcd a relevant message
void gameEnd() {
  address = 0;
  int value = EEPROM.read(address);

  if (score > value) {
    highScore = score;
    highScorePlayer = lastInputString;
    saveToEEPROM();
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("New high score!");
    
    lcd.setCursor(8, 1);
    lcd.print(highScore);
    delay(3000);
    
    } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Over"); 
    
    lcd.setCursor(0, 1);
    lcd.print("Try again?");
    }
    
    delay(4000);
    printGameDetails();
    
    lcd.setCursor(12, 0);
    lcd.print("Menu");

    lcd.setCursor(1, 1);
    lcd.print("Restart");
    resetGame();
}

// each time the player looses a life, a sad face is displayed and a relevant sound will warn him
void lifeLoss() {
  m.clear();
  m.writeSprite(0, 0, sadFace);
  noTone(passiveBuzzer);
  failSound();
  m.clear();
}

// shifts shape to the left 
void printShapeWithShift(byte shape[], int shiftSpeed) {
  m.writeSprite(8, 0, shape);
  score++;
  
  for (int i = 0; i < shape[0]; i++) { 
    
    m.setDot(col, row, 1);
    Jump();

     // if enough time has passed, shift the matrix, else decrement i so no column is missed
     if (millis() - lastShift > shiftSpeed){
        
       if (jumping) {
         if (millis() - lastJump > jumpingInterval) {
           m.setDot(col, row, 0);
           row -= jumpLength;  

           // when the loop is at the fifth repetition, the dot's column will start overlaping the columns of the matrix to be shifted     
           if (i >= 5) {
             // test if the dot landed on ground or if it hasn't hit an obstacle
             if ((shape[i - 3] & (1 << row)) != 0 || ((shape[i - 3] & 1) == 0)) { 
                noOfLives--;
                score--;
                lifeLoss();
                    
                i = shape[0]; // exit the loop
             }
           }  
           jumping = false;
          } else { 
            // test if while jumping no obstacle is encountered
            if (shape[i - 3] & (1 << row)) {
                noOfLives--;
                score --;
                lifeLoss();
                    
                i = shape[0]; // exit the loop
            } 
            m.setDot(col, row, 0);
          }
        } else {
          if (i >= 5)
            // test if the dot landed on ground or if it hasn't hit an obstacle 
            if ( (shape[i - 3] & (1 << row)) != 0 || ((shape[i - 3] & 1) == 0)) {
              noOfLives--;
              score--;
              lifeLoss();
              
              i = shape[0]; // exit the loop
            }
              
          m.setDot(col, row, 0);      
       }
        
       m.shiftLeft(false, false);
             
       lastShift = millis();
     }
     else
       i--; 

    if (noOfLives == 0) {
      gameState = false;
      m.clear();
      m.writeSprite(0, 0 , sadFace);
      gameEnd();
    }
  }
}

// number of lives left, level and score will be continuously updated and printed on the lcd throughout the game
void printGameDetails() {
  lcd.clear();
  
  for (int i = 0; i < 3; i++) {
    switch(i) {
      case 0: {
                lcd.setCursor(1, 0);
                lcd.print("Score");
                lcd.setCursor(7, 0);
                lcd.print(score);
                
                break;
      }
      case 1: {
                lcd.setCursor(1, 1);
                lcd.print("Lives");
                lcd.setCursor(7, 1);
                lcd.print(noOfLives);
                
                break;
      }
      case 2: {
                lcd.setCursor(9, 1);
                lcd.print("Level");
                lcd.setCursor(15, 1);
                lcd.print(level);
                
                break;
      }
    }    
 }
}

void enterName() {
  if (Serial.available() > 0) {
    incomingByte = (char)Serial.read();
    
    if (incomingByte == '\n') {
      lcd.setCursor(8, 0);
      lcd.print(inputString);
      
      // make sure to "clean up" when inserting a new name for player that has a smaller length than the last one
      for (int j = 1; j <= 8 - inputString.length(); j++) 
        lcd.print(" ");
        
      lastInputString = inputString;
      inputString = "";
    } else {
      inputString += incomingByte;
    }
  } else {
      lcd.setCursor(8, 0);
      lcd.print(lastInputString);
  }
}

void printHighScore() {
  // get and print high score
  address = 0;
  highScoreValue = EEPROM.read(address);
  address = address + 1;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(highScoreValue);

  // print player's name
  // if no string has been enter , then the player is unknown
  nameLength = EEPROM.read(address);
  if (nameLength == 0) {
    lcd.print(" ");
    lcd.print("Unknown");
  } else {
    char c[nameLength + 1];
    address++;
    
    for (int i = 0; i < nameLength; i++) {
       c[i] = EEPROM.read(address + i);
    }
    c[nameLength] = '\0';
    lcd.print(" ");
    lcd.print(c);
  }
}

void saveToEEPROM() {
  address = 0;
 
  EEPROM.write(address, highScore);
  address++;
  
  EEPROM.write(address, highScorePlayer.length());
  address++;  
  
  for (int i = 0; i < highScorePlayer.length(); i++) {
    EEPROM.write(address + i, highScorePlayer[i]);
  }
  EEPROM.write(address + highScorePlayer.length(), '\0');
}
