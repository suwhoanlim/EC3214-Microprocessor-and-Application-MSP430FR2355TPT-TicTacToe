#include <msp430.h> 


/*
 * functions
 */
void init_player_switch(); // init player switches
void init_board_pins(); // init tictactoe led pins
void init_analogue_sensor(); // init potentiometer
void init_dc_motor(); // init dc motor
void init_timer(); // init timer
void reset_everything(); // used for reset button
void determine_winner(); // used for timeout, or when the game is a tie, or when someone wins.
// TODO: MORE FUNCTIONS TO COME

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	init_player_switch(); // initialize switch for player1 and player2
	init_board_pins(); // initialize tictactoe led pins
	init_analogue_sensor(); // initialize analogue sensor(potentiometer) for player 1 and player2
	init_dc_motor(); // initialize dc motor to represent time
	init_timer(); // initialize timer for dc motor speed

	while(1) {
	    /*
	     * Use global variables to communicate between functions
	     *
	     * 1. always start with a press of reset switch
	     * 2. p1 or p2 presses their button, they will start first (say p1 pressed)
	     * 3. p1 adjusts potentiometer in given time, presses switch
	     * 4. once switch interrupt occurs, it reads the value of potentiometer
	     * 4.1 using switch-case loop, choose which led to light up with which color (if statement in switch)
	     * 4.2 light up the led
	     * 4.3 wait until the timer ends
	     * 000. when timer falls, check if user pressed the switch. if not, call determine_winner.
	     * if both pressed, report error and finish the game as a tie
	     * when user pressed the switch, change the led/motor direction
	     * 000. when timer falls, check for winner condition - 8 for win, 1 for tie
	     *
	     * reset switch
	     * - halt timer by mc = 0, clear timer TBCLR & &T0BCLR
	     * - turn off the leds in tictactoe board, clears the led for whose turn
	     *
	     * player switch
	     * - 1st press inits which led, starts timer
	     * - 2nd presses
	     */
	}

	return 0; // never reached
}

void init_player_switch() {

}
void init_board_pins(){

}
void init_analogue_sensor(){

}
void init_dc_motor(){
    // implement later
}
void init_timer(){
    // implement later
}
void reset_everything(){

}
void determine_winner(){

}

