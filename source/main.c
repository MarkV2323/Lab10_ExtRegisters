/*	Author: Mark Alan Vincent II
 *  Partner(s) Name: NA
 *	Lab Section: A01
 *	Assignment: Lab #10  Exercise #1
 *  Youtube:
 *	Exercise Description:
 *
 *  For this lab I need to setup a shift register and display the contents of
 *  char variable onto a bank of 8 LEDs. I should have two buttons that allow
 *  the incrementation and decrementation of the varible.
 *
 *
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

// Method for handling writing to register.
void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
   	 PORTC = 0x08;
   	 // set SER = next bit of data to be sent.
   	 PORTC |= ((data >> i) & 0x01);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTC |= 0x02;
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTC |= 0x04;
    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
}

/*
 *
 *
 *
*/

// globals (shared varibles for the SMs)
unsigned char tmpA = 0x00;


// Our concurrent synchSMs
enum SM1_States { SM1_Start } sm1_state;
int TickFct_BlinkLED(int state) {

    // Read Input from tmpA
    tmpA = ~PINA & 0xFF;

    // Transitions
    switch(state) {

        case SM1_Start:
            break;

        default:
            // Default
            state = SM1_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM1_Start:
            break;

        default:
            break;

    }

    // End of Tick Function SM1
    return state;
}


// Main Funcntion
int main(void) {

    // PORTS
    DDRA = 0x00; PORTA = 0xFF; // PortA as input
    DDRB = 0xFF; PORTB = 0x00; // PortB as output
    DDRC = 0xFF; PORTC = 0x00; // PortC as output

    // Setup Task List
    unsigned char i = 0;

    // SM1
    //tasks[i].state = SM1_Start;
	  //tasks[i].period = 1500;
	  //tasks[i].elapsedTime = tasks[i].period;
	  //tasks[i].TickFct = &TickFct_BlinkLED;
	  //i++;

    // Setup System Period & Timer to ON.
    TimerSet(500);
    TimerOn();
    while (1) {}
    return 1;
}
