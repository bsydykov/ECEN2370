# ECEN2370  FINAL PROJECT

This project implements the classic Tetris game on the STM32F429I-Discovery microcontroller. It leverages the board's peripherals, including the LCD screen, hardware buttons, and a hardware random number generator (RNG). The game is structured into several modules, each handling a specific aspect of the game, ensuring clarity and modularity.

This project was all about bringing the timeless Tetris game to life on the STM32F429I-Discovery board. I wanted to make a playable and engaging version of Tetris while taking full advantage of the board's capabilities, like the built-in LCD screen, hardware buttons, and the TIM7 timer. The project combined a lot of moving parts—both literally and figuratively—from designing smooth tetromino movements and rotations to implementing a timer that accurately tracks gameplay and displays it at the end. Along the way, I focused on creating an experience that feels like classic Tetris, complete with row-clearing mechanics and a polished game-over screen.

The project has three main screens:

Start Screen: Displays a "Press to Play" message and a preview of all Tetromino shapes.
Gameplay Screen: Features the Tetris playfield, with interactive Tetrominoes that fall and respond to player inputs.
End Screen: Shows a "Game Over" message and the total elapsed playtime using TIM7.


Code Contributions

main.c
The main file is where everything kicks off. It’s responsible for setting up the system clocks, initializing peripherals, and launching the game through ApplicationInit(). Most of the heavy lifting happens in other files, but I added the logic to transition between screens and handle game state flags.

ApplicationCode.c
This file is like the central hub of the project. It’s where I set up all the hardware components, like the touch screen, buttons, and timers. One of my favorite things I wrote here was the code for the "Start Screen," where the Tetromino shapes are displayed in their respective colors. It’s simple but sets the tone for the game. I also added the logic to detect when the screen is tapped to move from the start screen to gameplay.

Game_Driver.c
This file is where the magic happens. It handles all the game logic, like moving Tetrominoes, rotating them, and detecting when rows are complete. I set up the Tetromino shapes as 4x4 matrices and used collision detection to ensure pieces interacted correctly with the board and each other. Writing the logic to clear rows and spawn new pieces was tricky, but it was satisfying once it all worked smoothly.

LCD_Driver.c
This was the fun part: making the game look good. I used this file to handle all the drawing on the LCD screen, from the Tetromino blocks to the grid. One highlight is the visualDemo() function, which draws the start screen with animations and the “Press to Play” message. I also wrote a helper function to draw individual blocks in the game, which made it easy to update the screen during gameplay.

RNG.c
The RNG file is small but essential. It’s where I initialized the random number generator and wrote a simple function to generate random numbers. These numbers are used to pick Tetromino shapes, ensuring each game feels unique and challenging.

Button_Driver.c
This file handles input from the physical button on the board. I set it up as an interrupt so you can rotate Tetrominoes in real time during gameplay. It’s a simple addition, but it adds a lot to the interactivity of the game.

QA

Struggles and Obstacles?
One of the biggest challenges was configuring TIM7 for accurate time tracking. Initially, the timer was not incrementing the elapsed time correctly due to misconfigurations in the interrupt handler. Debugging this required adding print statements and experimenting with timer settings until it worked as expected.
Another issue was aligning tetromino data structures with the drawing functions. The initial implementation caused pointer type mismatches, resulting in rendering errors. This was resolved by refining the data structure and ensuring compatibility between function arguments and array types.
Finally, balancing real-time game updates with smooth LCD rendering required careful performance testing and optimizations. Rapid button inputs occasionally caused glitches, which I addressed by adding debouncing logic in the button driver.

What I Learned?
This project taught me the importance of modular design and the value of debugging tools like print statements and LEDs in embedded systems. I gained hands-on experience configuring hardware timers, managing interrupts, and integrating multiple peripherals. I also learned how to approach performance optimization in resource-constrained environments.

What I Would Do Differently?
If I were to restart this project, I would allocate more time to planning the initial architecture. A more detailed understanding of the LCD and timer functionalities upfront could have saved significant debugging time. Additionally, I would implement automated testing for critical modules to catch edge cases earlier in development.

How the Project Could Be Improved?
To improve this project further, I could add features like a scoring system and increasing difficulty levels to make the game more engaging. Incorporating sound effects and animations for row clearing would improve the user experience. Saving high scores in non-volatile memory and adding a pause/resume feature would also add value.
