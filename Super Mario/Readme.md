Super Mario

The game is a simpler, pixelated version of Super Mario Bros, where Mario is actually represented by a dot on the 8x8 LED Matrix that by default moves further, can either jump (as the surface he walks on is not continuous nor leveled), or shoot with a limited number of "flames"(3 per game) to clear his way when jumping seems like a more difficult alternative. The game is over in either of these two cases: Not being able to jump over an obstacle and therefore getting stucked (the screen will scroll further leaving Mario behind), or falling into the abysm.

Requirements:

      - Score: Initially, the score is zero, and a certain value will be added to it each five seconds. As the speed increases, so will the value added to the score, and successfully shooting obstacles will also bring extra points. Therefore, the more minutes have passed, the faster the score will increase.
      
      - Progress in difficulty: With every minute, the speed increases, meaning that the obstacles and shapes displayed on the matrix will be scrolled faster. For example, the shapes displayed will be shifted each 400 milliseconds initially. Gradually, the amount of time between the shifts will decrease, implying an increase in the speed of the game.
      
      - High Score: The Highest Score achieved will be updated after a game if it's the case and saved in EEPROM so that it can be displayed on the LCD when the corresponding submenu is selected.
      
      - Extra: Once the game starts, a buzzer will play the Super Mario theme song until the game is over.
