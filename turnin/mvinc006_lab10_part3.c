/*	Author: Mark Alan Vincent II
 *  Partner(s) Name: NA
 *	Lab Section: A01
 *	Assignment: Lab #10  Exercise #3
 *  Youtube:  https://youtu.be/NvGlcWlY1Pc
 *	Exercise Description:
 *
 *  In this Exercise, I need to create 3 festive light displays that can be
 *  cycled through using the shift register. 2 buttons should handle the shifting
 *  of the displays, and both pressed together should turn the system on or off.
 *
 *  In this part, I also need to add another shift register and add functionality
 *  for playing two festive themes at the same time.
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
// Modified: RCLK (C2 and C4) and SRCLR(C3 and C5) are independent of each other.
void transmit_data(unsigned char data, unsigned char regi) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
     if (regi == 1) {
       PORTC = 0x08;
     } else if (regi == 2) {
       PORTC = 0x20;
     }
   	 // set SER = next bit of data to be sent.
   	 PORTC |= ((data >> i) & 0x01);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTC |= 0x02;
    }

    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    if (regi == 1) {
      PORTC |= 0x04;
    } else if (regi == 2) {
      PORTC |= 0x10;
    }

    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
}

// globals (shared varibles for the SMs)
unsigned char tmpA = 0x00;

unsigned char lights = 0x00;
unsigned char lightsVal = 0x00;
unsigned char seqCount = 0;

unsigned char lights2 = 0x00;
unsigned char lightsVal2 = 0x00;
unsigned char seqCount2 = 0;

// Light sequences
unsigned char seqLights1[13] =
{0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0xFF, 0x00, 0xFF, 0x00, 0x00};

unsigned char seqLights2[13] =
{0xF0, 0x0F, 0xF0, 0x0F, 0xFF, 0x00, 0xFF, 0xFF, 0x66, 0x99, 0x66, 0x99, 0x00};

unsigned char seqLights3[13] =
{0x7E, 0x3C, 0x18, 0x00, 0xFF, 0xAA, 0x55, 0xAA, 0x55, 0x00, 0x01, 0x80, 0x00};

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
            // Init value to 0.
            lights = 0x00;
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
          // Next display
          if (lights < 2) {
            lights = lights + 1;
          } else {
            lights = 0;
          }
          break;

      case SM1_A1:
          // Previous display
          if (lights > 0) {
            lights = lights - 1;
          } else {
            lights = 2;
          }
          break;

      case SM1_Both:
          // Turns the lights off or on depending on value.
          if ( (lights >= 0) && (lights <= 2) ) {
            // turns lights off
            lights = 10;
          } else if (lights == 10) {
            // turns lights on
            lights = 0;
          }
          break;

      default:
          break;

    }

    // End of Tick Function SM1
    return state;
}

enum SM2_States { SM2_Start, SM2_Off, SM2_Light1, SM2_Light2, SM2_Light3 } sm2_state;
int TickFct_Lights(int state) {

    // Transitions
    switch(state) {

        case SM2_Start:
            // Go to the off state
            state = SM2_Off;
            seqCount = 0;
            break;

        case SM2_Off:
            if (lights == 0) {
              state = SM2_Light1;
              seqCount = 0;
            } else {
              state = SM2_Off;
            }
            break;

        case SM2_Light1:
            if (lights == 10) {
              state = SM2_Off;
            } else if (lights == 0) {
              state = SM2_Light1;
            } else if (lights == 1) {
              state = SM2_Light2;
              seqCount = 0;
            } else if (lights == 2) {
              state = SM2_Light3;
              seqCount = 0;
            } else {
              // Bad Light value, return to off
              state = SM2_Off;
            }
            break;

        case SM2_Light2:
            if (lights == 10) {
              state = SM2_Off;
            } else if (lights == 0) {
              state = SM2_Light1;
              seqCount = 0;
            } else if (lights == 1) {
              state = SM2_Light2;
            } else if (lights == 2) {
              state = SM2_Light3;
              seqCount = 0;
            } else {
              // Bad Light value, return to off
              state = SM2_Off;
            }
            break;

        case SM2_Light3:
            if (lights == 10) {
              state = SM2_Off;
            } else if (lights == 0) {
              state = SM2_Light1;
              seqCount = 0;
            } else if (lights == 1) {
              state = SM2_Light2;
              seqCount = 0;
            } else if (lights == 2) {
              state = SM2_Light3;
            } else {
              // Bad Light value, return to off
              state = SM2_Off;
            }
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

        case SM2_Off:
            // Set lights to offline
            lightsVal = 0x00;
            // Set count to 0.
            seqCount = 0;
            break;

        case SM2_Light1:
            // looping through festive light sequence.
            if (seqCount < 13) {
                lightsVal = seqLights1[seqCount];
                seqCount = seqCount + 1;
            } else if (seqCount == 13) {
                // reset seqCount
                seqCount = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal = seqLights1[seqCount];
                seqCount = seqCount + 1;
            } else {
                // something has gone wrong, reset.
                seqCount = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal = seqLights1[seqCount];
                seqCount = seqCount + 1;
            }

            break;

        case SM2_Light2:
            // looping through festive light sequence.
            if (seqCount < 13) {
                lightsVal = seqLights2[seqCount];
                seqCount = seqCount + 1;
            } else if (seqCount == 13) {
                // reset seqCount
                seqCount = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal = seqLights2[seqCount];
                seqCount = seqCount + 1;
            } else {
                // something has gone wrong, reset.
                seqCount = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal = seqLights2[seqCount];
                seqCount = seqCount + 1;
            }
            break;

        case SM2_Light3:
            // looping through festive light sequence.
            // looping through festive light sequence.
            if (seqCount < 13) {
                lightsVal = seqLights3[seqCount];
                seqCount = seqCount + 1;
            } else if (seqCount == 13) {
                // reset seqCount
                seqCount = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal = seqLights3[seqCount];
                seqCount = seqCount + 1;
            } else {
                // something has gone wrong, reset.
                seqCount = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal = seqLights3[seqCount];
                seqCount = seqCount + 1;
            }
            break;

        default:
            break;
    }

    // End of Tick Function SM1
    return state;
}

enum SM3_States { SM3_Start, SM3_Write } sm3_state;
int TickFct_Write(int state) {

    // Transitions
    switch(state) {

        case SM3_Start:
            // Goes to the SM2_Write state.
            state = SM3_Write;
            break;

        case SM3_Write:
            // Loops on SM2_Write.
            state = SM3_Write;
            break;

        default:
            // Default
            state = SM3_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM3_Start:
            break;

        case SM3_Write:
            // Call the method to write to the register.
            transmit_data(lightsVal, 1);
            break;

        default:
            break;

    }

    // End of Tick Function SM1
    return state;
}

// Clones of our SMs for register 2
enum SM4_States { SM4_Start, SM4_Off, SM4_A2, SM4_A3, SM4_Both } sm4_state;
int TickFct_Button2(int state) {

    // Read Input from tmpA
    tmpA = ~PINA & 0xFF;

    // Transitions
    switch(state) {

        case SM4_Start:
            // Go to the off state
            state = SM4_Off;
            // Init value to 0.
            lights2 = 0x00;
            break;

        case SM4_Off:
            // Check input
            if (tmpA == 0x00) {
                // Stay in SM1_Off
                state = SM4_Off;
            } else if (tmpA == 0x04) {
                // A2 activated
                state = SM4_A2;
            } else if (tmpA == 0x08) {
                // A3 activated
                state = SM4_A3;
            } else if (tmpA == 0x0C) {
                // A2 and A3 are activated
                state = SM4_Both;
            } else {
                // Bad input, do nothing.
                state = SM4_Off;
            }
            break;

        case SM4_A2:
            // Check input
            if (tmpA == 0x00) {
                // Stay in SM1_Off
                state = SM4_Off;
            } else if (tmpA == 0x04) {
                // A2 activated
                state = SM4_A2;
            } else if (tmpA == 0x08) {
                // A3 activated
                state = SM4_A3;
            } else if (tmpA == 0x0C) {
                // A2 and A3 are activated
                state = SM4_Both;
            } else {
                // Bad input, do nothing.
                state = SM4_Off;
            }
            break;

        case SM4_A3:
            // Check input
            if (tmpA == 0x00) {
                // Stay in SM1_Off
                state = SM4_Off;
            } else if (tmpA == 0x04) {
                // A2 activated
                state = SM4_A2;
            } else if (tmpA == 0x08) {
                // A3 activated
                state = SM4_A3;
            } else if (tmpA == 0x0C) {
                // A2 and A3 are activated
                state = SM4_Both;
            } else {
                // Bad input, do nothing.
                state = SM4_Off;
            }
            break;

        case SM4_Both:
            // Check input
            if (tmpA == 0x00) {
                // Stay in SM1_Off
                state = SM4_Off;
            } else if (tmpA == 0x04) {
                // A2 activated
                state = SM4_A2;
            } else if (tmpA == 0x08) {
                // A3 activated
                state = SM4_A3;
            } else if (tmpA == 0x0C) {
                // A2 and A3 are activated
                state = SM4_Both;
            } else {
                // Bad input, do nothing.
                state = SM4_Off;
            }
            break;

        default:
            // Default
            state = SM4_Start;
            break;

    }

    // Actions
    switch(state) {

      case SM4_Start:
          break;

      case SM4_Off:
          break;

      case SM4_A2:
          // Next display
          if (lights2 < 2) {
            lights2 = lights2 + 1;
          } else {
            lights2 = 0;
          }
          break;

      case SM4_A3:
          // Previous display
          if (lights2 > 0) {
            lights2 = lights2 - 1;
          } else {
            lights2 = 2;
          }
          break;

      case SM4_Both:
          // Turns the lights off or on depending on value.
          if ( (lights2 >= 0) && (lights2 <= 2) ) {
            // turns lights off
            lights2 = 10;
          } else if (lights2 == 10) {
            // turns lights on
            lights2 = 0;
          }
          break;

      default:
          break;

    }

    // End of Tick Function SM1
    return state;
}

enum SM5_States { SM5_Start, SM5_Off, SM5_Light1, SM5_Light2, SM5_Light3 } sm5_state;
int TickFct_Lights2(int state) {

    // Transitions
    switch(state) {

        case SM5_Start:
            // Go to the off state
            state = SM5_Off;
            seqCount2 = 0;
            break;

        case SM5_Off:
            if (lights2 == 0) {
              state = SM5_Light1;
              seqCount2 = 0;
            } else {
              state = SM5_Off;
            }
            break;

        case SM5_Light1:
            if (lights2 == 10) {
              state = SM5_Off;
            } else if (lights2 == 0) {
              state = SM5_Light1;
            } else if (lights2 == 1) {
              state = SM5_Light2;
              seqCount2 = 0;
            } else if (lights2 == 2) {
              state = SM5_Light3;
              seqCount2 = 0;
            } else {
              // Bad Light value, return to off
              state = SM5_Off;
            }
            break;

        case SM5_Light2:
            if (lights2 == 10) {
              state = SM5_Off;
            } else if (lights2 == 0) {
              state = SM5_Light1;
              seqCount2 = 0;
            } else if (lights2 == 1) {
              state = SM5_Light2;
            } else if (lights2 == 2) {
              state = SM5_Light3;
              seqCount2 = 0;
            } else {
              // Bad Light value, return to off
              state = SM5_Off;
            }
            break;

        case SM5_Light3:
            if (lights2 == 10) {
              state = SM5_Off;
            } else if (lights2 == 0) {
              state = SM5_Light1;
              seqCount2 = 0;
            } else if (lights2 == 1) {
              state = SM5_Light2;
              seqCount2 = 0;
            } else if (lights2 == 2) {
              state = SM5_Light3;
            } else {
              // Bad Light value, return to off
              state = SM5_Off;
            }
            break;

        default:
            // Default
            state = SM5_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM5_Start:
            break;

        case SM5_Off:
            // Set lights to offline
            lightsVal2 = 0x00;
            // Set count to 0.
            seqCount2 = 0;
            break;

        case SM5_Light1:
            // looping through festive light sequence.
            if (seqCount2 < 13) {
                lightsVal2 = seqLights1[seqCount2];
                seqCount2 = seqCount2 + 1;
            } else if (seqCount2 == 13) {
                // reset seqCount
                seqCount2 = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal2 = seqLights1[seqCount2];
                seqCount2 = seqCount2 + 1;
            } else {
                // something has gone wrong, reset.
                seqCount2 = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal2 = seqLights1[seqCount2];
                seqCount2 = seqCount2 + 1;
            }

            break;

        case SM5_Light2:
            // looping through festive light sequence.
            if (seqCount2 < 13) {
                lightsVal2 = seqLights2[seqCount2];
                seqCount2 = seqCount2 + 1;
            } else if (seqCount2 == 13) {
                // reset seqCount
                seqCount2 = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal2 = seqLights2[seqCount2];
                seqCount2 = seqCount2 + 1;
            } else {
                // something has gone wrong, reset.
                seqCount2 = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal2 = seqLights2[seqCount2];
                seqCount2 = seqCount2 + 1;
            }
            break;

        case SM5_Light3:
            // looping through festive light sequence.
            // looping through festive light sequence.
            if (seqCount2 < 13) {
                lightsVal2 = seqLights3[seqCount2];
                seqCount2 = seqCount2 + 1;
            } else if (seqCount2 == 13) {
                // reset seqCount
                seqCount2 = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal2 = seqLights3[seqCount2];
                seqCount2 = seqCount2 + 1;
            } else {
                // something has gone wrong, reset.
                seqCount2 = 0;
                // Do first iteration now that seqCount = 0.
                lightsVal2 = seqLights3[seqCount2];
                seqCount2 = seqCount2 + 1;
            }
            break;

        default:
            break;
    }

    // End of Tick Function SM1
    return state;
}

enum SM6_States { SM6_Start, SM6_Write } sm6_state;
int TickFct_Write2(int state) {

    // Transitions
    switch(state) {

        case SM6_Start:
            // Goes to the SM2_Write state.
            state = SM6_Write;
            break;

        case SM6_Write:
            // Loops on SM6_Write.
            state = SM6_Write;
            break;

        default:
            // Default
            state = SM6_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM6_Start:
            break;

        case SM6_Write:
            // Call the method to write to the register.
            transmit_data(lightsVal2, 2);
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

    // SM1
    tasks[i].state = SM1_Start;
	  tasks[i].period = 100;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Button;
	  i++;

    // SM4
    tasks[i].state = SM4_Start;
	  tasks[i].period = 100;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Button2;
	  i++;

    // SM2
    tasks[i].state = SM2_Start;
	  tasks[i].period = 200;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Lights;
	  i++;

    // SM5
    tasks[i].state = SM5_Start;
	  tasks[i].period = 200;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Lights2;
	  i++;

    // SM3
    tasks[i].state = SM3_Start;
	  tasks[i].period = 100;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Write;
    i++;

    // SM6
    tasks[i].state = SM6_Start;
	  tasks[i].period = 100;
	  tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Write2;

    // Setup System Period & Timer to ON.
    TimerSet(tasksGCD);
    TimerOn();
    while (1) {}
    return 1;
}
