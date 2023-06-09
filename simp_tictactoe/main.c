#include <msp430.h> 
#include <math.h>


void init_player_switch(); // init player switches
void init_board_pins(); // init tictactoe led pins
void init_analogue_sensor(); // init potentiometer

void init_pwm();
void init_encoder();
void init_timer();

void test_led_on(); // Test if LED work properly
void test_led_off(); // Test if LED work properly

int b1 = 0;
int b2 = 0;
int b3 = 0;
int r1 = 0;
int r2 = 0;
int r3 = 0;
int ADC_value = 0;
int test_b = 0;
int test_r = 0;
int test_it = 0;

int EncoderCount = 0;
float Duty = 0.2;


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	init_player_switch();
	init_board_pins();
	init_analogue_sensor();
	init_pwm();
	init_encoder();
	init_timer();

    __enable_interrupt();
    PM5CTL0 &= ~LOCKLPM5;

    //test_led_on();
    test_led_off(); // turn all the LED off on start


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

    test_it++;

    P4IFG &= ~BIT1;
}

#pragma vector = PORT3_VECTOR
__interrupt void ISR_player1_switch_pressed() { // P6.0, 6.1, 6.2
    //int ADC_value;

    test_r++;

    ADCCTL0 |= ADCENC | ADCSC; // start reading ADC value
    while((ADCIFG & ADCIFG0) == 0); // wait untill conversion is finished

    ADC_value = ADCMEM0; // MAX 3974? Need to check with debugger


    /*
     * Toggling LED
     * If LED is off, turn it on
     * If LED is on, turn it off
     */
    if(ADC_value < 1300) { // MAX ADC value = 4095, 4095 / 3 == 1300
        P6OUT ^=BIT0; // if LED is on, turn it off
    }
    else if ((1300 <= ADC_value) && (ADC_value < 2600)){
        P6OUT ^= BIT1;
    }
    else if(2600 <= ADC_value) {
        P6OUT ^= BIT2;
    }
    /*
     * Code discarded. It either doesn't work or is too complicated.
     *
    if(ADC_value < 1300) {
        if(P6OUT & BIT0 != 0) { // check if LED is on
            P6OUT &= ~BIT0; // if LED is on, turn it off
        }
        else {
            P6OUT |= BIT0; // if LED is off, turn it on
        }
    }
    else if ((1300 <= ADC_value) && (ADC_value < 2600)){
        if(P6OUT & BIT1 != 0) {
            P6OUT &= ~BIT1;
        }
        else {
            P6OUT |= BIT1;
        }
    }
    else if(2600 <= ADC_value) {
        if(P6OUT & BIT2 != 0) {
            P6OUT &= ~BIT2;
        }
        else {
            P6OUT |= BIT2;
        }
    }
    */
    P3IFG &= ~BIT0;
}

#pragma vector = PORT2_VECTOR
__interrupt void ISR_player2_switch_pressed() { // P2.0 2.2 4.0
    //int ADC_value;

    /*
     * if all three LED are on, turn them off and end ISR
     */

    /*
    if((P6OUT & BIT0) && (P2OUT & BIT2) && (P4OUT & BIT0)) {
        test ++;
        return;
    }
    */
    test_b++;

    if(b1 == 1) {
        if(b2 == 1) {
            if(b3 == 1){
                P2OUT &= ~BIT0;
                P2OUT &= ~BIT2;
                P4OUT &= ~BIT0;
                b1 = 0;
                b2 = 0;
                b3 = 0;

                P2IFG &= ~BIT5;
                return;
            }
        }
    }

    /*
     * Else turn on the LED. If it is on, ignore signal and keep it turned on
     */
    ADCCTL0 |= ADCENC | ADCSC; // start reading ADC value
    while((ADCIFG & ADCIFG0) == 0); // wait untill conversion is finished

    ADC_value = ADCMEM0; // MAX 3974? Need to check with debugger

    if(ADC_value < 1300) {
        P2OUT |= BIT0;
        b1 = 1;
    }
    else if ((1300 <= ADC_value) && (ADC_value < 2600)){
        P2OUT |= BIT2;
        b2 = 1;
    }
    else if(2600 <= ADC_value) {
        P4OUT |= BIT0;
        b3 = 1;
    }
    int i = 0;
    for(i = 0x0FFFF; i>0; i--) {} //delay

    P2IFG &= ~BIT5;
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
    //SpeedMeas = EncoderCount*0.625;         // Calculate the motor speed from the pulse count

    TB0CCR1 = (int)(999*Duty);      // Determine TB0 CCR1
    EncoderCount = 0;               // Reset the pulse count
    TB1CCTL0 &= ~CCIFG;             // Clear TB1 CCR0 Flag
}


#pragma vector = TIMER2_B0_VECTOR   //TB2CCR0
__interrupt void ISR_TB2_CCR0(void)
{

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

    //SpeedCmd = 30;
    Duty = 0.3;
    TB2CCTL0 &= ~CCIFG;
}

#pragma vector = TIMER2_B1_VECTOR   //TB2CCR1
__interrupt void ISR_TB2_CCR(void)
{
    if (TB2CCTL1 & CCIFG){
        //SpeedCmd = 60;
        Duty = 0.5;
        TB2CCTL1 &= ~CCIFG;}
    else if(TB2CCTL2 & CCIFG){
        //SpeedCmd = 120;
        Duty = 0.9;
        TB2CCTL2 &= ~CCIFG;}
}


/*
 * Below are Test codes and init codes
 */

void test_led_on() {
    /*
     * This function will turn on all the LED
     */

    /*
     * Red LED
     */
    P6OUT &= ~BIT0;
    P6OUT |= BIT1;
    P6OUT &= ~BIT2;

    /*
     * Blue LED
     */
    P2OUT |= BIT0;
    P2OUT &= ~BIT2;
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

void init_pwm(){
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

void init_encoder(void){
    P5DIR &= ~BIT0;             // Configure P5.0(Encoder) as input
    P5IES &= ~BIT0;             // Configure IRQ Sensitivity H-to-L

    P5IFG &= ~BIT0;             // Clear P5.0(Encoder) IRQ Flag 5.0
    P5IE |= BIT0;
}

void init_timer(void){
    TB1CTL |= TBCLR;            // Clear TB1
    TB1CTL |= TBSSEL__ACLK;     // Source = ACLK (32.768kHz)
    TB1CTL |= MC__UP;           // Mode = Up
    TB1CCR0 = 3276;             // CCR0 = 3276 -> Control Frequency = 32.768kHz/(3276 + 1) = 10 Hz

    TB1CCTL0 |= CCIE;           // Enable TB1 CCR0 IRQ
    TB1CCTL0 &= ~CCIFG;         // Clear TB1 CCR0 Flag

    TB2CTL |= TBCLR;            // Clear TB2
    TB2CTL |= TBSSEL__ACLK;     // Source = ACLK (32.768kHz)
    TB2EX0 |= TBIDEX_4;         // Divide by 5 by using IDEX -> 1 cycle = 10s
    TB2CTL |= MC__UP;            // Mode = UP
    TB2CCR0 = 65530;            // CCR0 = 65535 -> Control Frequency 32.768kHz/5 * 65535 ~= 10s
    TB2CCR1 = 21845;            // CCR1 = 43691 -> Control Frequency 32.768kHz/5 * 43691 ~= 3s
    TB2CCR2 = 43691;

    TB2CCTL0 |= CCIE;           // Enable TB2 CCR0 IRQ
    TB2CCTL0 &= ~CCIFG;         // Clear TB2 CCR0 Flag
    TB2CCTL1 |= CCIE;           // Enable TB2 CCR1 IRQ
    TB2CCTL1 &= ~CCIFG;         // Clear TB2 CCR1 Flag
    TB2CCTL2 |= CCIE;           // Enable TB2 CCR2 IRQ
    TB2CCTL2 &= ~CCIFG;         // Clear TB2 CCR2 Flag
}
