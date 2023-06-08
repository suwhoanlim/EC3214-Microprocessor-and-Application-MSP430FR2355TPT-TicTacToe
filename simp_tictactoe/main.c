#include <msp430.h> 
#include <math.h>


void init_player_switch(); // init player switches
void init_board_pins(); // init tictactoe led pins
void init_analogue_sensor(); // init potentiometer
void test_led_on(); // Test if LED work properly
void test_led_off(); // Test if LED work properly

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	init_player_switch();
	init_board_pins();
	init_analogue_sensor();

	//turn_led_on();
	//turn_led_off();

    __enable_interrupt();
    PM5CTL0 &= ~LOCKLPM5;

	while(1) {
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
         *
         * - (Yet to come)
         * - For DC motor, every time TB2 CCR0 is hit, each player's one LED will be turned off.
         *
         * Now, please enjoy!
         *
         */
	}
	return 0;
}

#pragma vector = PORT4_VECTOR
__interrupt void ISR_turn_LED_off() { // P6.0, 6.1, 6.2 // P2.0, 2.2, 4.0
    /*
     * Once Pressed, turn off all the LED
     */

    /*
     * Red LED
     */
    P6OUT &= ~BIT0;
    P6OUT &= ~BIT1;
    P6OUT &= ~BIT2;

    /*
     * Blue LED
     */
    P2OUT &= ~BIT0;
    P2OUT &= ~BIT2;
    P4OUT &= ~BIT0;
}

#pragma vector = PORT3_VECTOR
__interrupt void ISR_player1_switch_pressed() { // P6.0, 6.1, 6.2
    int ADC_value;

    ADCCTL0 |= ADCENC | ADCSC; // start reading ADC value
    while((ADCIFG & ADCIFG0) == 0); // wait untill conversion is finished

    ADC_value = ADCMEM0; // MAX 3974? Need to check with debugger


    /*
     * Toggling LED
     * If LED is off, turn it on
     * If LED is on, turn it off
     */
    if(ADC_value < 1300) {
        if(P6OUT & BIT0) { // check if LED is on
            P6OUT &= ~BIT0; // if LED is on, turn it off
        }
        else {
            P6OUT |= BIT0; // if LED is off, turn it on
        }
    }
    else if ((1300 <= ADC_value) && (2600 < ADC_value)){
        if(P6OUT & BIT1) {
            P6OUT &= ~BIT1;
        }
        else {
            P6OUT |= BIT1;
        }
    }
    else if(2600 <= ADC_value) {
        if(P6OUT & BIT2) {
            P6OUT &= ~BIT2;
        }
        else {
            P6OUT |= BIT2;
        }
    }
}

#pragma vector = PORT2_VECTOR
__interrupt void ISR_player2_switch_pressed() { // P2.0 2.2 4.0
    int ADC_value;

    /*
     * if all three LED are on, turn them off and end ISR
     */
    if((P6OUT & BIT0) && (P2OUT & BIT2) && (P4OUT & BIT0)) {
        P2OUT &= ~BIT0;
        P2OUT &= ~BIT2;
        P4OUT &= ~BIT0;
        return;
    }

    /*
     * Else turn on the LED. If it is on, ignore signal and keep it turned on
     */
    ADCCTL0 |= ADCENC | ADCSC; // start reading ADC value
    while((ADCIFG & ADCIFG0) == 0); // wait untill conversion is finished

    ADC_value = ADCMEM0; // MAX 3974? Need to check with debugger

    if(ADC_value < 1300) {
        P2OUT |= BIT0;
    }
    else if ((1300 <= ADC_value) && (2600 < ADC_value)){
        P2OUT |= BIT2;
    }
    else if(2600 <= ADC_value) {
        P4OUT |= BIT0;
    }

}

void test_led_on() {
    /*
     * This function will turn on all the LED
     */

    /*
     * Red LED
     */
    P6OUT |= BIT0;
    P6OUT |= BIT1;
    P6OUT |= BIT2;

    /*
     * Blue LED
     */
    P2OUT |= BIT0;
    P2OUT |= BIT2;
    P4OUT |= BIT0;
}

void test_led_off() {
    /*
     * This function will turn off all the LED
     */

    /*
     * Red LED
     */
    P6OUT &= ~BIT0;
    P6OUT &= ~BIT1;
    P6OUT &= ~BIT2;

    /*
     * Blue LED
     */
    P2OUT &= ~BIT0;
    P2OUT &= ~BIT2;
    P4OUT &= ~BIT0;
}

void init_player_switch() {
    /*
     * P3.0 for player 1
     * P2.5 for Player 2
     * P4.1 for some other occation - probablly reset
     */

    P3DIR &= ~BIT0; // Configure as input
    P2DIR &= ~BIT5;
    P4DIR &= ~BIT1; // for reset

    P3REN |= BIT0; // enable pull up/down
    P2REN |= BIT5;
    P4REN |= BIT1; // for reset

    P3OUT &= ~BIT0; // enable pull down, 0 when nothing is pressed, 1 when pressed.
    P2OUT &= ~BIT5;
    P4OUT &= ~BIT1; // for reset

    /*
     * Interrupt inits
     */
    P3IES &= ~BIT0; // interrupt for L-to-H
    P2IES &= ~BIT5;
    P4IES &= ~BIT1;

    P3IFG &= ~BIT0; // Clear port for IRQ Flag
    P2IFG &= ~BIT5;
    P4IFG &= ~BIT1;

    P3IE |= BIT0; // Enable port IRQ
    P2IE |= BIT5;
    P4IE |= BIT1;
}

void init_board_pins() {
    /*
     * Red LED
     */
    P6DIR |= BIT0; // Configuring as output
    P6DIR |= BIT1;
    P6DIR |= BIT2;

    /*
     * Blue LED
     */
    P2DIR |= BIT0;
    P2DIR |= BIT2;
    P4DIR |= BIT0;
}

void init_analogue_sensor() {
    /*
     * Use A2 (P1.2) port
     */
    P1SEL1 |= BIT2;
    P1SEL0 |= BIT2;

    PM5CTL0 &= ~LOCKLPM5;

    ADCCTL0 &= ~ADCSHT;
    ADCCTL0 |= ADCSHT_2;
    ADCCTL0 |= ADCON;

    ADCCTL1 |= ADCSSEL_2;
    ADCCTL1 |= ADCSHP;
    ADCCTL2 &= ~ADCRES;
    ADCCTL2 |= ADCRES_2;

    ADCMCTL0 |= ADCINCH_2; // Use channel A2(P1.2)

    //ADCIE |= ADCIE0; // comment out the interrupt flag. We want to manually read ADC value
}
