/*	Author: Mark Alan Vincent II
 *  Partner(s) Name: NA
 *	Lab Section: A01
 *	Assignment: Lab #10  Exercise #1
 *  Youtube:  https://youtu.be/rkREuB4CCCA
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

// globals (shared varibles for the SMs)
unsigned char tmpA = 0x00;
unsigned char valueA = 0x00;

// Our concurrent synchSMs
enum SM1_States { SM1_Start, SM1_Off, SM1_A0, SM1_A1, SM1_Both } sm1_state;
int TickFct_Button(int state) {

    // Read Input from tmpA
    tmpA = ~PINA & 0xFF;

    // Transitions
    switch(state) {

        case SM1_Start:
            // Go to the off state
            state = SM1_Off;
            // Init value to 7. (3 LEDs are on)
            valueA = 0x07;
            break;

        case SM1_Off:
            // Check input
            if (tmpA == 0x00) {
                // Stay in SM1_Off
                state = SM1_Off;
            } else if (tmpA == 0x01) {
                // A0 activated
                state = SM1_A0;
            } else if (tmpA == 0x02) {
                // A1 activated
                state = SM1_A1;
            } else if (tmpA == 0x03) {
                // A0 and A1 are activated
                state = SM1_Both;
            } else {
                // Bad input, do nothing.
                state = SM1_Off;
            }
            break;

        case SM1_A0:
            if (tmpA == 0x00) {
                // go to SM1_Off
                state = SM1_Off;
            } else if (tmpA == 0x01) {
                // A0 activated
                state = SM1_A0;
            } else if (tmpA == 0x02) {
                // A1 activated
                state = SM1_A1;
            } else if (tmpA == 0x03) {
                // A0 and A1 are activated
                state = SM1_Both;
            } else {
                // Bad input, do nothing.
                state = SM1_A0;
            }
            break;

        case SM1_A1:
            if (tmpA == 0x00) {
                // go to SM1_Off
                state = SM1_Off;
            } else if (tmpA == 0x01) {
                // A0 activated
                state = SM1_A0;
            } else if (tmpA == 0x02) {
                // A1 activated
                state = SM1_A1;
            } else if (tmpA == 0x03) {
                // A0 and A1 are activated
                state = SM1_Both;
            } else {
                // Bad input, do nothing.
                state = SM1_A1;
            }
            break;

        case SM1_Both:
            if (tmpA == 0x00) {
                // go to SM1_Off
                state = SM1_Off;
            } else if (tmpA == 0x01) {
                // A0 activated
                state = SM1_A0;
            } else if (tmpA == 0x02) {
                // A1 activated
                state = SM1_A1;
            } else if (tmpA == 0x03) {
                // A0 and A1 are activated
                state = SM1_Both;
            } else {
                // Bad input, do nothing.
                state = SM1_Both;
            }
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

      case SM1_Off:
          break;

      case SM1_A0:
          // Check if valueA < 0xFF
          if (valueA < 0xFF) {
            valueA = valueA + 1;
          }
          break;

      case SM1_A1:
          // Check if valueA > 0x00
          if (valueA > 0x00) {
            valueA = valueA - 1;
          }
          break;

      case SM1_Both:
          // Reset our value.
          valueA = 0x00;
          break;

      default:
          break;

    }

    // End of Tick Function SM1
    return state;
}

enum SM2_States { SM2_Start, SM2_Write } sm2_state;
int TickFct_Write(int state) {

    // Read Input from tmpA
    tmpA = ~PINA & 0xFF;

    // Transitions
    switch(state) {

        case SM2_Start:
            // Goes to the SM2_Write state.
            state = SM2_Write;
            break;

        case SM2_Write:
            // Loops on SM2_Write.
            state = SM2_Write;
            break;

        default:
            // Default
            state = SM2_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM2_Start:
            break;

        case SM2_Write:
            // Call the method to write to the register.
            transmit_data(valueA);
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
    DDRC = 0xFF; PORTC = 0x00; // PortC as output

    // Setup Task List
    unsigned char i = 0;

    // SM1 (Handles button presses and increment / decrement)
    tasks[i].state = SM1_Start;
	  tasks[i].period = 100; // 300ms button presses.
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Button;
	  i++;

    // SM2 (Handles writing valueA to the shift register)
    tasks[i].state = SM2_Start;
    tasks[i].period = 100; // 300ms button presses.
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &TickFct_Write;

    // Setup System Period & Timer to ON.
    TimerSet(tasksGCD);
    TimerOn();
    while (1) {}
    return 1;
}
