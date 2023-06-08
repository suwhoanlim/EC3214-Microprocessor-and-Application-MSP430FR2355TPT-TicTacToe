#include <msp430.h> 
#include <math.h>

/*
 * Global variables
 */

int running_status = 0; // for which led to light up
int player1_turn_end = 0;
int player2_turn_end = 0;
int whoseturn = 0; // 1 for p2's turn, 0 for p1's turn
int on_led = 0;
int game_end = 0;
int z = 0;
int p1int = 0;
int p2int = 0;
int tb2int = 0;

//Some below are never used.
//----------- Parameters -----------//

//----------------------------------//

//int SW1;
int EncoderCount = 0;
float Duty = 0.2;
//float DutyPI = 0;
//float Error = 0;
//float ErrorSum = 0;
//float SpeedMeas = 0; //unit: RPM



/*
 * functionsf
 */
void init_player_switch(); // init player switches
void init_board_pins(); // init tictactoe led pins
void init_analogue_sensor(); // init potentiometer
void init_timer(); // init timer
//void reset_everything(); // used for reset button, impelemented on interrupt?
//void determine_winner(); // used for timeout, or when the game is a tie, or when someone wins, implemented on timer?
void init_encoder(); // used to config dc motor
void init_PWM();
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
	init_timer(); // initialize timer for dc motor speed
	init_PWM();
	init_encoder();

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

	 // never reached
}

void init_player_switch() {
    /*
     * P4.6 = Player1,  P4.7 = Player2 switch input
     * P3.7 for reset button
     */
    P4DIR &= ~BIT6; // Configure as input
    P4DIR &= ~BIT7;
    P2DIR &= ~BIT3; // for reset

    P4REN |= BIT6; // enable pull up/down
    P4REN |= BIT7;
    P2REN |= BIT3; // for reset

    P4OUT &= ~BIT6; // enable pull down, 0 when nothing is pressed, 1 when pressed.
    P4OUT &= ~BIT7;
    P2OUT &= ~BIT3; // for reset

    /*
     * Interrupt inits
     */
    P4IES &= ~BIT6; // interrupt for L-to-H
    P4IES &= ~BIT7;
    P2IES &= ~BIT3;

    P4IFG &= ~BIT6; // Clear port for IRQ Flag
    P4IFG &= ~BIT7;
    P2IFG &= ~BIT3;

    P4IE |= BIT6; // Enable port IRQ
    P4IE |= BIT7;
    P2IE |= BIT3;

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
    P2DIR |= BIT1;
    P2DIR |= BIT4;

    P4DIR |= BIT4;
    P3DIR |= BIT3;
    P2DIR |= BIT5;
    P3DIR |= BIT2;
    P3DIR |= BIT0;

    P3DIR |= BIT4;
    P1DIR |= BIT1;
    P1DIR |= BIT3;
    P3DIR |= BIT5;
    P1DIR |= BIT2;
    P3DIR |= BIT1;

    P4DIR |= BIT5; // player1 scoreboard
    P5DIR |= BIT4; // Player2 scoreboard
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

void init_encoder(){
    P5DIR &= ~BIT0;             // Configure P5.0(Encoder) as input
    P5IES &= ~BIT0;             // Configure IRQ Sensitivity H-to-L
}

void init_PWM(){
    P5DIR |= BIT2;              // Configure P1.6 as output
    P5OUT |= BIT2;              // Set the initial value as 1

    TB0CTL |= TBCLR;            // Clear TB0
    TB0CTL |= TBSSEL__SMCLK;    // Source = SMCLK (1Mhz)
    TB0CTL |= MC__UP;           // Mode = Up
    TB0CCR0 = 999;              // CCR0 = 999 -> PWM Frequency = 1MHz/(999 + 1) = 1 kHz
    TB0CCR1 = 0;                // CCR1 = 0, initialization

    TB0CCTL0 |= CCIE;           // Enable TB0 CCR0 IRQ
    TB0CCTL0 &= ~CCIFG;         // Clear TB0 CCR0 Flag
    TB0CCTL1 |= CCIE;           // Enable TB0 CCR1 IRQ
    TB0CCTL1 &= ~CCIFG;         // Clear TB0 CCR1 Flag
}

void init_timer(){
    // code merged
    TB1CTL |= TBCLR;            // Clear TB1
    TB1CTL |= TBSSEL__ACLK;     // Source = ACLK (32.768kHz)
    TB1CTL |= MC__UP;           // Mode = Up
    TB1CCR0 = 3276;             // CCR0 = 3276 -> Control Frequency = 32.768kHz/(3276 + 1) = 10 Hz

    TB1CCTL0 |= CCIE;           // Enable TB1 CCR0 IRQ
    TB1CCTL0 &= ~CCIFG;         // Clear TB1 CCR0 Flag

    TB2CTL |= TBCLR;            // Clear TB2
    TB2CTL |= TBSSEL__ACLK;     // Source = ACLK (32.768kHz)
    TB2EX0 |= TBIDEX_4;         // Divide by 5 by using IDEX -> 1 cycle = 10s
    TB2CTL |= MC__UP;            // Mode = CONTINUOUS
    TB2CCR0 = 65530;            // CCR0 = 65535 -> Control Frequency 32.768kHz/5 * 65535 ~= 10s
    TB2CCR1 = 21845;            // CCR1 = 43691 -> Control Frequency 32.768kHz/5 * 43691 ~= 3s
    TB2CCR2 = 43691;

    //TB2CTL |= TBIE;             // Enable TB2 overflow IRQ
    //TB2CTL &= ~TBIFG;           // Clear TB2 overflow Flag

    TB2CCTL0 |= CCIE;           // Enable TB2 CCR0 IRQ
    TB2CCTL0 &= ~CCIFG;         // Clear TB2 CCR0 Flag
    TB2CCTL1 |= CCIE;           // Enable TB2 CCR1 IRQ
    TB2CCTL1 &= ~CCIFG;         // Clear TB2 CCR1 Flag
    TB2CCTL2 |= CCIE;           // Enable TB2 CCR2 IRQ
    TB2CCTL2 &= ~CCIFG;         // Clear TB2 CCR2 Flag
}

#pragma vector = PORT4_VECTOR
__interrupt void ISR_player_switch_pressed() {

    int ADC_Value1 = 0;
    int ADC_Value2 = 0;

    // P4.6 = Player1,  P4.7 = Player2 switch input

    if(P4IFG & BIT6) { // switch pressed by player 1
        if(running_status == 0) {
            p1int -= 10;
            running_status = 1;
            /* TODO
             * implement starting timer code here
             */
            Duty = 0.2;
            TB2CTL |= TBSSEL__ACLK;     // Source = ACLK (32.768kHz)
            TB2EX0 |= TBIDEX_4;         // Divide by 5 by using IDEX -> 1 cycle = 10s
            TB2CTL |= MC__UP;            // Mode = CONTINUOUS
            TB2CCR0 = 65530;            // CCR0 = 65535 -> Control Frequency 32.768kHz/5 * 65535 ~= 10s
            TB2CCR1 = 21845;            // CCR1 = 43691 -> Control Frequency 32.768kHz/5 * 43691 ~= 3s
            TB2CCR2 = 43691;

            P5OUT &= ~BIT4;
            P4OUT |= BIT5; // turn on scoreboard
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
            P2OUT |= BIT4;
        }
        else if((1365 <= ADC_Value1 && ADC_Value1 < 2730) && (1365 <= ADC_Value2 && ADC_Value2 < 2730)) {
            P3OUT |= BIT3;
        }
        else if((1365 <= ADC_Value1  && ADC_Value1 < 2730) && (2730 <= ADC_Value2)) {
            P3OUT |= BIT2;
        }
        else if(2730 <= ADC_Value1 && ADC_Value2 < 1365) {
            P3OUT |= BIT4;
        }
        else if(2730 <= ADC_Value1 && (1365 <= ADC_Value2  && ADC_Value2 < 2730)) {
            P1OUT |= BIT3;
        }
        else if(2730 <= ADC_Value1 && (2730 <= ADC_Value2)) {
            P1OUT |= BIT2;
        }
        player1_turn_end = 1;
        player2_turn_end = 0;
        P4IFG &= ~BIT6; // reset flag
        on_led++;
        p1int ++;
    }
    else if(P4IFG & BIT7) { // interrupt by plyer2
        if(running_status == 0) {
            running_status = 1;
            /* TODO
             * implement starting timer code here
             */
            Duty = 0.2;
            TB2CTL |= TBSSEL__ACLK;     // Source = ACLK (32.768kHz)
            TB2EX0 |= TBIDEX_4;         // Divide by 5 by using IDEX -> 1 cycle = 10s
            TB2CTL |= MC__UP;            // Mode = CONTINUOUS
            TB2CCR0 = 65530;            // CCR0 = 65535 -> Control Frequency 32.768kHz/5 * 65535 ~= 10s
            TB2CCR1 = 21845;            // CCR1 = 43691 -> Control Frequency 32.768kHz/5 * 43691 ~= 3s
            TB2CCR2 = 43691;

            P4OUT &= ~BIT5;
            P5OUT |= BIT4; // turn on scoreboard
            P4IFG &= ~BIT7; // reset flag
            whoseturn = 1;
            p2int -= 10;
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
            P2OUT |= BIT1;
        }
        else if((1365 <= ADC_Value1  && ADC_Value1 < 2730) && ADC_Value2 < 1365) {
            P4OUT |= BIT4;
        }
        else if((1365 <= ADC_Value1  && ADC_Value1 < 2730) && (1365 <= ADC_Value2  && ADC_Value2 < 2730)) {
            P2OUT |= BIT5;
        }
        else if((1365 <= ADC_Value1  && ADC_Value1 < 2730) && (2730 <= ADC_Value2)) {
            P3OUT |= BIT0;
        }
        else if(2730 <= ADC_Value1 && ADC_Value2 < 1365) {
            P1OUT |= BIT1;
        }
        else if(2730 <= ADC_Value1 && (1365 <= ADC_Value2  && ADC_Value2 < 2730)) {
            P3OUT |= BIT5;
        }
        else if(2730 <= ADC_Value1 && (2730 <= ADC_Value2)) {
            P3OUT |= BIT1;
        }
        player2_turn_end = 1;
        player1_turn_end = 0;
        P4IFG &= ~BIT7; // reset flag
        on_led++;
        p2int ++;
    }
}

#pragma vector = PORT2_VECTOR
__interrupt void ISR_reset_switch_pressed() {
    /*
     * reset switch
    * - halt timer by mc = 0, clear timer TBCLR & &T0BCLR
    * - turn off the leds in tictactoe board, clears the led for whose turn
*/

    running_status = 0;
    TB2CTL |= MC__STOP; // stop timer
    TB2CTL |= TBCLR; // clear timer and dividers
    Duty = 0.5;

    /* Turn off LEDs */
    int i = 0;
    int bit;
    for(i = 4; i >= 0; i--) {
        bit = 1 << i;
        P6OUT &= ~bit; // turn off LED
    }
    P2OUT &= ~BIT1;
    P2OUT &= ~BIT4;
    P2OUT &= ~BIT5;
    P1OUT &= ~BIT1;
    P1OUT &= ~BIT2;
    P1OUT &= ~BIT3;
    P4OUT &= ~BIT4;

    for(i = 5; i >= 0; i--) {
        bit = 1 << i;
        P3OUT &= ~bit; // turn off LED
    }
    whoseturn = -1;
    player1_turn_end = -1;
    player2_turn_end = -1;
    on_led = 0;
    //TODO turn on both LED on scoreboard?
    // turn on both LED, give it some delay, and turn it off to indicate board is ready

    if(game_end == 0) { // reset interrupt happend before game end
        P4OUT &= BIT5;
        P5OUT &= BIT4;
    }
    game_end = 0;
    i = 0xFFFF;
    /*
    for(i=0xFFFF;i > 0; i--) {} // delay to display result

    for(i=0xFFFF;i > 0; i--) {} // delay to display result

    for(i=0xFFFF;i > 0; i--) {} // delay to display result

    for(i=0xFFFF;i > 0; i--) {} // delay to display result
    */
    P4OUT &= ~BIT5;
    P5OUT &= ~BIT4;


    /* Clear interrupt flags */
    TB2CCTL0 &= ~CCIFG;
    TB2CCTL1 &= ~CCIFG;
    TB2CCTL2 &= ~CCIFG;


    P4IFG &= ~BIT6;
    P4IFG &= ~BIT7;
    P2IFG &= ~BIT3;
    z ++;
}

#pragma vector = TIMER2_B0_VECTOR   //TB2CCR0
__interrupt void ISR_TB2_CCR0(void) {
    tb2int += 10;
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
        //player 2 wins due to timeout
        P5OUT |= BIT4;
        P4OUT &= ~BIT5;
        game_end = 1;
        P2IFG |= BIT3; // set reset switch flag
    }
    else if(whoseturn==1 && player2_turn_end != 1) {
        //player 1 wins due to timeout
        P4OUT |= BIT5;
        P5OUT &= ~BIT4;
        game_end = 1;
        P2IFG |= BIT3; // set reset switch flag
    }
    else if( // Check win condition for player1
            (P6OUT & BIT0) && (P6OUT & BIT2) && (P6OUT & BIT4) ||
            (P2OUT & BIT4) && (P3OUT & BIT3) && (P3OUT & BIT2) ||
            (P3OUT & BIT4) && (P1OUT & BIT3) && (P1OUT & BIT2) ||
            (P6OUT & BIT0) && (P2OUT & BIT4) && (P3OUT & BIT4) ||
            (P6OUT & BIT2) && (P3OUT & BIT3) && (P1OUT & BIT3) ||
            (P6OUT & BIT4) && (P3OUT & BIT2) && (P1OUT & BIT2) ||
            (P6OUT & BIT0) && (P3OUT & BIT3) && (P1OUT & BIT2) ||
            (P3OUT & BIT4) && (P3OUT & BIT3) && (P6OUT & BIT4)
    ) {
        P3OUT |= BIT5;
        P3OUT &= ~BIT6;
        game_end = 1;
        P2IFG |= BIT3; // set reset switch flag
    }
    else if( // Check win condition for player2
            (P6OUT & BIT1) && (P6OUT & BIT3) && (P2OUT & BIT1) ||
            (P4OUT & BIT4) && (P2OUT & BIT5) && (P3OUT & BIT0) ||
            (P1OUT & BIT1) && (P3OUT & BIT5) && (P3OUT & BIT1) ||
            (P6OUT & BIT1) && (P4OUT & BIT4) && (P1OUT & BIT1) ||
            (P6OUT & BIT3) && (P2OUT & BIT4) && (P3OUT & BIT5) ||
            (P2OUT & BIT1) && (P3OUT & BIT0) && (P3OUT & BIT1) ||
            (P6OUT & BIT1) && (P2OUT & BIT5) && (P3OUT & BIT1) ||
            (P1OUT & BIT1) && (P2OUT & BIT5) && (P2OUT & BIT1)
    ) {
        P5OUT |= BIT4;
        P5OUT &= ~BIT4;
        game_end = 1;
        P2IFG |= BIT3; // set reset switch flag
    }
    else if(on_led == 9) { // game ends in a tie
        P5OUT |= BIT4;
        P5OUT |= BIT4;
        game_end = 1;
        P2IFG |= BIT3; // set reset switch flag
        on_led = 0;
    }
    else { // move to next turn
        P5OUT ^= BIT4;
        P4OUT ^= BIT5;
        if(whoseturn == 0 ) whoseturn = 1;
        else whoseturn = 1;
    }

    //set reset interrupt
    //set global variable to tell reset interrupt that game just ended
    //TB0CCTL0 &= ~CCIFG; // clear ccr0 flag
    Duty = 0.2;
    TB2CCTL0 &= ~CCIFG;
}

/* ISR for Encoder (Pulse counting) */
#pragma vector=PORT1_VECTOR // THIS CODE SHOULD NOT WORK... IN FACT THIS ISR WILL NEVER BE EXECUTED??
__interrupt void EncoderISR(void)
{
    EncoderCount++;
    P5IFG &= ~BIT0;
}

/* ISR for PWM (Pulse generation) */
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    P5OUT |= BIT2;
    TB0CCTL0 &= ~CCIFG;
}
#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_CCR1(void)
{
    P5OUT &= ~BIT2;
    TB0CCTL1 &= ~CCIFG;
}

/* ISR for Timer (Speed control) */
#pragma vector = TIMER1_B0_VECTOR
__interrupt void ISR_TB1_CCR0(void) {
    SpeedMeas = EncoderCount*0.625;         // Calculate the motor speed from the pulse count

    TB0CCR1 = (int)(999*Duty);      // Determine TB0 CCR1
    EncoderCount = 0;               // Reset the pulse count
    TB1CCTL0 &= ~CCIFG;             // Clear TB1 CCR0 Flag
}

#pragma vector = TIMER2_B1_VECTOR   //TB2CCR1
__interrupt void ISR_TB2_CCR(void)
{
    /*
    tb2int ++;
    if (TB2CCTL1 & CCIFG){
        //SpeedCmd = 60;
        Duty = 0.4;
        TB2CCTL1 &= ~CCIFG;}
    else if(TB2CCTL2 & CCIFG){
        //SpeedCmd = 120;
        Duty = 0.8;
        TB2CCTL2 &= ~CCIFG;}
*/
}

