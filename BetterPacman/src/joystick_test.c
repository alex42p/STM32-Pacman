/*____________________________________________________________________________________________________________________

****************************************************************************************
Function name: joystick_test.c
Writer: Omkar Ghodke 
Function goal: Testing the joystick_pos.c function using UART
****************************************************************************************

****************************************************************************************
Function needs:
    1) Need to use UART to print the values of the joystick
    2) Need to use a timer to read the values at a certain interval

Important dates and updates: 
    - 3/16/2025 -> Building the pesudo-code and researching on Joysticks
                -> Reference 1 -> https://www.youtube.com/watch?v=BNqL7t7a9W4 **This reference is not good**
                -> Reference 2 -> https://www.youtube.com/watch?v=umkD1piCNvc 

_______________________________________________________________________________________________________________________*/

#include <stdio.h>
#include "stm32f0xx.h"

// need to initialize the UART 

void init_usart5() {
    // TODO
    
    //enables clocks for GPIOC and GPIOD
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIODEN; 

    //code below sets PC12 to USART5_TX
    GPIOC->MODER &= ~(GPIO_MODER_MODER12);
    GPIOC->MODER |= (GPIO_MODER_MODER12_1); // sets PC12 in alternate mode

    GPIOC->AFR[1] &= ~(GPIO_AFRH_AFSEL12);
    GPIOC->AFR[1] |= 2 << GPIO_AFRH_AFSEL12_Pos;

    //code below sets PD2 to USART5_TX
    GPIOD->MODER &= ~(GPIO_MODER_MODER2);
    GPIOD->MODER |= (GPIO_MODER_MODER2_1); // sets PC12 in alternate mode

    GPIOD->AFR[0] &= ~(GPIO_AFRL_AFSEL2);
    GPIOD->AFR[0] |= 2 << GPIO_AFRL_AFSEL2_Pos;

    RCC->APB1ENR |= RCC_APB1ENR_USART5EN; //enables clock for USART5

    USART5->CR1 &= ~(USART_CR1_UE); 
    USART5->CR1 &= ~(USART_CR1_M); //sets the word size to 8 bit
    USART5->CR2 &= ~(USART_CR2_STOP_0 | USART_CR2_STOP_1); //stops 1 bit
    USART5->CR1 &= ~(USART_CR1_PCE); //sets it for no parity control
    USART5->CR1 &= ~(USART_CR1_OVER8); // oversampling by 16
    USART5->BRR = 0x30; //0x1A1; //baud rate set to 115.2
    USART5->CR1 |= (USART_CR1_TE | USART_CR1_RE); //enables receiver and transmitter
    USART5->CR1 |= (USART_CR1_UE);

    while(!(USART5->ISR & (USART_ISR_TEACK | USART_ISR_REACK))); //waits for the TEACK bit
    //while(!(USART5->ISR & USART_ISR_REACK)); //waits for the REACK bit
}

void uart_send_char(char c) {
    while (!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
}

void uart_send_string(const char *s) {
    while (*s) {
        uart_send_char(*s++);
    }
}

void uart_send_float(float value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f", value);
    uart_send_string(buffer);
}

