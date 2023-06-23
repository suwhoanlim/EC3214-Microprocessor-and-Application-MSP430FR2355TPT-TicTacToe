# EC3214-Microprocessor-and-Application-MSP430FR2355TPT-TicTacToe
Implemenation of the game Tic-Tac-Toe using MSP430FR2355TPT

There are two project folders in this repo. `tictactoe` folder is the one where I tried to implement the entire game of tic-tac-toe using MSP430. Not only I configured the pins, but also I implemented the game logic as well as all the neccessary interrupts, timers, digital I/O, etc. Nevertheless, due to issue occured during implementation of game logic combined with lack of time, the folder is deprecated and has not been debugged since.

In `simp_tictactoe`, which is a fully functioning project, I have implemented a simplified version of tic-tac-toe. I have reduced the number of LEDs from 18(9 LEDs for each player) down to 6(3 LEDs for each player). Detailed logic of how the project works are depicted inside the `main.c` file, which I have added below;

        /*
         * This is a simplified version of game TicTacToe
         *
         * The following items were used:
         * - 6 LEDs(3 for player1, 3 for player2)
         * - 3 switches(1 for each player, and 1 for reset)
         * - 1 Analogue sensor
         * - 1 DC motor
         *
         * Following techniques were used;
         * - Digital I/O (turning LED on / off)
         * - Interrupt (when user presses the switch)
         * - ADC (reads value from the sensor and act accordingly depending on the result)
         * - Timer / PWM (Used to control the speed of DC motor, speed of DC motor will change periodically with the preset value)
         * - DC motor control (Manipulate the speed by changing Duty variable)
         * - ... and programming with C language!
         *
         * This program works like the following;
         * - When switch is pressed, value from the analogue sensor is read.
         * - For player1, switch will toggle the LED with the corresponding ADC value.
         * - For player2, once all three LED are lit, next press will turn every LED off no matter the ADC value.
         * - For reset button, it will clear all the LED and turn them off.
         * - For DC motor, every time TB2 CCR0 is hit, each player's one LED will be turned off.
         *
         * Now, please enjoy!
         *
         */

Just for future reference, I have added a rather lousy description of pin configuration. Some parts of the description is written in poorly hand-written Korean. The rule of Thumb is, black writing represents which gadget is plugged into which pin of the board. If there is no black writing, follow the writing with magenta color.

[Tictactoe_pin_layout_230623_222138584.pdf](https://github.com/suwhoanlim/EC3214-Microprocessor-and-Application-MSP430FR2355TPT-TicTacToe/files/11848828/Tictactoe_pin_layout_230623_222138584.pdf)
