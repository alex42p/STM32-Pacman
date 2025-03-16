/*____________________________________________________________________________________________________________________

****************************************************************************************
Function name: joystick_pos.c
Writer: Omkar Ghodke
Function goal: Reading the input from the Joystick as an Analog number and returning
               the number
****************************************************************************************

****************************************************************************************
Function needs:
    1) Needs to use 2 analog pins for analog value in the x and y direction (PA 1 and 2 for now)
    2) Need one more pin for the switch button of the joystick 

Important dates and updates: 
    - 3/16/2025 -> Building the pesudo-code and researching on Joysticks
                -> Reference 1 -> https://www.youtube.com/watch?v=BNqL7t7a9W4 **This reference is not good**
                -> Reference 2 -> https://www.youtube.com/watch?v=umkD1piCNvc 

_______________________________________________________________________________________________________________________*/


// Turn the RCC clock on for Port A
// Configure PA 1 and 2 for analog mode
// Set it up to be ADC as done in lab 4
// boxcar average should be used to smooth out the value being read

