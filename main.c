#include <msp430.h> 

/*
 * Global variables
 */

int ADC_Value1 = 0; // remove if doesn't seem neccessary
int ADC_Value2 = 0;
int running_status = 0; // for which led to light up
int player1_turn_end = 0;
int player2_turn_end = 0;
int whoseturn = 0; // 1 for p2's turn, 0 for p1's turn
int on_led = 0;



/*
 * functions
 */
void init_player_switch(); // init player switches
void init_board_pins(); // init tictactoe led pins
void init_analogue_sensor(); // init potentiometer
void init_dc_motor(); // init dc motor
void init_timer(); // init timer
void reset_everything(); // used for reset button, impelemented on interrupt?
void determine_winner(); // used for timeout, or when the game is a tie, or when someone wins, implemented on timer?
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

	__enable_interrupt();
	PM5CTL0 &= ~LOCKLPM5;

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
    /*
     * P4.6 = Player1,  P4.7 = Player2 switch input
     * P3.7 for reset button
     */
    P4DIR &= ~BIT6; // Configure as input
    P4DIR &= ~BIT7;
    P3DIR &= ~BIT7; // for reset

    P4REN |= BIT6; // enable pull up/down
    P4REN |= BIT7;
    P3REN |= BIT7; // for reset

    P4OUT &= ~BIT6; // enable pull down, 0 when nothing is pressed, 1 when pressed.
    P4OUT &= ~BIT7;
    P3OUT &= ~BIT7; // for reset

    /*
     * Interrupt inits
     */
    P4IES &= ~BIT6; // interrupt for L-to-H
    P4IES &= ~BIT7;
    P3IES &= ~BIT7;

    P4IFG &= ~BIT6; // Clear port for IRQ Flag
    P4IFG &= ~BIT7;
    P3IFG &= ~BIT7;

    P4IE |= BIT6; // Enable port IRQ
    P4IE |= BIT7;
    P3IE |= BIT7;

}
void init_board_pins(){
    /*
     * Uses P6.0~6, P5.0~4, P4.0~5 as LED output
     */
    P6DIR |= BIT0; // Configuring as output
    P6DIR |= BIT1;
    P6DIR |= BIT2;
    P6DIR |= BIT3;
    P6DIR |= BIT4;
    P6DIR |= BIT5;
    P6DIR |= BIT6;

    P5DIR |= BIT0;
    P5DIR |= BIT1;
    P5DIR |= BIT2;
    P5DIR |= BIT3;
    P5DIR |= BIT4;

    P4DIR |= BIT0;
    P4DIR |= BIT1;
    P4DIR |= BIT2;
    P4DIR |= BIT3;
    P4DIR |= BIT4;
    P4DIR |= BIT5;

    P3DIR |= BIT5; // player1
    P3DIR |= BIT6; // Player2


}
void init_analogue_sensor(){
    /*
     * P1.4, P1.5 for player1
     * P1.6, P1.7 for player2
     */

    P1SEL1 |= BIT4; // Configure P1.4 for A4
    P1SEL0 |= BIT4;

    P1SEL1 |= BIT5; // Configure P1.5 for A5
    P1SEL0 |= BIT5;

    P1SEL1 |= BIT6; // Configure P1.6 for A6
    P1SEL0 |= BIT6;

    P1SEL1 |= BIT7; // Configure P1.7 for A7
    P1SEL0 |= BIT7;

    ADCCTL0 &= ~ADCSHT; // Configuring ADC, only need to be done once?!
    ADCCTL0 |= ADCSHT_2;
    ADCCTL0 |= ADCON;
    ADCCTL1 |= ADCSSEL_2;
    ADCCTL1 |= ADCSHP;
    ADCCTL2 &= ~ADCRES;
    ADCCTL2 |= ADCRES_2;

    //ADCMCTL0 |= ADCINCH_2; // ADC input channel = A2(P1.2)
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

#pragma vector = PORT4_VECTOR
__interrupt void ISR_player_switch_pressed() {


    // P4.6 = Player1,  P4.7 = Player2 switch input

    if(P4IFG & BIT6) { // switch pressed by player 1
        if(running_status == 0) {
            running_status = 1;
            /* TODO
             * implement starting timer code here
             */

            P3OUT &= ~BIT6;
            P3OUT |= BIT5; // turn on scoreboard
            P4IFG &= ~BIT6; // reset flag
            whoseturn = 0;
            return;
        }
        ADCMCTL0 |= ADCINCH_4; // ADC input channel = A4(P1.4)

        ADCCTL0 |= ADCENC | ADCSC; // enable and start
        while((ADCIFG & ADCIFG0) == 0); // wait until ADC conversion ends
        ADC_Value1 = ADCMEM0; // save the value into ADC_val

        ADCMCTL0 |= ADCINCH_5; // ADC input channel = A5(P1.5)

        ADCCTL0 |= ADCENC | ADCSC; // enable and start
        while((ADCIFG & ADCIFG0) == 0); // wait until ADC conversion ends
        ADC_Value2 = ADCMEM0; // save the value into ADC_val

        /*
         * turn corresponding LED on
         */
        if(ADC_Value1 < 1365 && ADC_Value2 < 1365) {
            P6OUT |= BIT0;
        }
        else if(ADC_Value1 < 1365 && (1365 <= ADC_Value2  && ADC_Value2 < 2730)) {
            P6OUT |= BIT2;
        }
        else if(ADC_Value1 < 1365 && 2730 <= ADC_Value2) {
            P6OUT |= BIT4;
        }
        else if((1365 <= ADC_Value1 && ADC_Value1 < 2730) && ADC_Value2 < 1365) {
            P6OUT |= BIT6;
        }
        else if((1365 <= ADC_Value1 && ADC_Value1 < 2730) && (1365 <= ADC_Value2 && ADC_Value2 < 2730)) {
            P5OUT |= BIT1;
        }
        else if((1365 <= ADC_Value1  && ADC_Value1 < 2730) && (2730 <= ADC_Value2)) {
            P5OUT |= BIT3;
        }
        else if(2730 <= ADC_Value1 && ADC_Value2 < 1365) {
            P4OUT |= BIT0;
        }
        else if(2730 <= ADC_Value1 && (1365 <= ADC_Value2  && ADC_Value2 < 2730)) {
            P4OUT |= BIT2;
        }
        else if(2730 <= ADC_Value1 && (2730 <= ADC_Value2)) {
            P4OUT |= BIT5;
        }
        player1_turn_end = 1;
        whoseturn = 1;
        P4IFG &= ~BIT6; // reset flag
        on_led++;
    }
    else if(P4IFG & BIT7) { // interrupt by plyer2
        if(running_status == 0) {
            running_status = 1;
            /* TODO
             * implement starting timer code here
             */
            P3OUT &= ~BIT5;
            P3OUT |= BIT6; // turn on scoreboard
            P4IFG &= ~BIT7; // reset flag
            whoseturn = 1;
            return;
        }
        ADCMCTL0 |= ADCINCH_6; // ADC input channel = A6(P1.6)

        ADCCTL0 |= ADCENC | ADCSC; // enable and start
        while((ADCIFG & ADCIFG0) == 0); // wait until ADC conversion ends
        ADC_Value1 = ADCMEM0; // save the value into ADC_val

        ADCMCTL0 |= ADCINCH_7; // ADC input channel = A7(P1.7)

        ADCCTL0 |= ADCENC | ADCSC; // enable and start
        while((ADCIFG & ADCIFG0) == 0); // wait until ADC conversion ends
        ADC_Value2 = ADCMEM0; // save the value into ADC_val

        /*
         * turn corresponding LED on
         */
        if(ADC_Value1 < 1365 && ADC_Value2 < 1365) {
            P6OUT |= BIT1;
        }
        else if(ADC_Value1 < 1365 && (1365 <= ADC_Value2  && ADC_Value2 < 2730)) {
            P6OUT |= BIT3;
        }
        else if(ADC_Value1 < 1365 && (2730 <= ADC_Value2)) {
            P6OUT |= BIT5;
        }
        else if((1365 <= ADC_Value1  && ADC_Value1 < 2730) && ADC_Value2 < 1365) {
            P5OUT |= BIT0;
        }
        else if((1365 <= ADC_Value1  && ADC_Value1 < 2730) && (1365 <= ADC_Value2  && ADC_Value2 < 2730)) {
            P5OUT |= BIT2;
        }
        else if((1365 <= ADC_Value1  && ADC_Value1 < 2730) && (2730 <= ADC_Value2)) {
            P5OUT |= BIT4;
        }
        else if(2730 <= ADC_Value1 && ADC_Value2 < 1365) {
            P4OUT |= BIT1;
        }
        else if(2730 <= ADC_Value1 && (1365 <= ADC_Value2  && ADC_Value2 < 2730)) {
            P4OUT |= BIT3;
        }
        else if(2730 <= ADC_Value1 && (2730 <= ADC_Value2)) {
            P4OUT |= BIT5;
        }
        player2_turn_end = 1;
        whoseturn = 0;
        P4IFG &= ~BIT7; // reset flag
        on_led++;
    }
}

#pragma vector = PORT3_VECTOR
__interrupt void ISR_reset_switch_pressed() {
    /*
     * reset switch
    * - halt timer by mc = 0, clear timer TBCLR & &T0BCLR
    * - turn off the leds in tictactoe board, clears the led for whose turn
*/
    running_status = 0;
    TB0CTL |= MC__STOP; // stop timer
    TB0CTL |= TBCLR; // clear timer and dividers

    /* Turn off LEDs */
    int i = 0;
    int bit;
    for(i = 6; i >= 0; i--) {
        bit = 1 << i;
        P6OUT &= ~bit; // turn off LED
    }
    for(i = 4; i >= 0; i--) {
        bit = 1 << i;
        P5OUT &= ~bit; // turn off LED
    }
    for(i = 5; i >= 0; i--) {
        bit = 1 << i;
        P4OUT &= ~bit; // turn off LED
    }
    whoseturn = -1;
    player1_turn_end = -1;
    player2_turn_end = -1;
    on_led = 0;
    //TODO turn on both LED on scoreboard?

    /* Clear interrupt flags */
    TB0CTL &= ~TBIFG;
    P4IFG &= ~BIT6;
    P4IFG &= ~BIT7;
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0() {
    /*
     * TODO
     * implement determine_winner here
     * 1. p1 win because of p2 timeout
     * 2. p2 win because of p1 timeout
     * 3. p1 win because made 3 crossing (check condition: 8)
     * 4. p2 win because made 3 crossing
     * 5. tie because no crossing was made
     */
    if(whoseturn==0 && player1_turn_end != 1) {
        //player 2 wins
    }
    else if(whoseturn==1 && player2_turn_end != 1) {
        //player 1 wins
    }
    else if(on_led == 9) {
        //tie
        on_led = 0;
    }

    TB0CCTL0 &= ~CCIFG; // clear ccr0 flag
}
