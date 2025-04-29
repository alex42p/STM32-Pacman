/**
 * @file: joystick_pos.c
 * @authors: Omkar Ghodke / Zoey Halligan
 * 
 * @description: 
 *          This file handles reading the input from the Joystick as an Analog number and returning that number
 *          Function needs:
 *          1) Needs to use 2 analog pins for analog value in the x and y direction (PA 1 and 2 for now)
 *          2) Need one more pin for the switch button of the joystick 
 *          -> Reference 1 -> https://www.youtube.com/watch?v=BNqL7t7a9W4 **This reference is not good**
 *          -> Reference 2 -> https://www.youtube.com/watch?v=umkD1piCNvc -             
 *
 */

#include "stm32f0xx.h"
#include <stdio.h>
extern void uart_send_float();
extern void uart_send_string();
// Turn the RCC clock on for Port A
// Configure PA 1 and 2 for analog mode
// Configure PA 3 to be a input (takes in the input of the button)
// Set it up to be ADC as done in lab 4
// boxcar average should be used to smooth out the value being read

/*PseudoCode:
    1) Set two ADC channels --> ADC_IN1(PA1) and ADC_IN2(PA2)
    2) Use DMA to set transfer for the two channels
*/

void joystickPin_config(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~(GPIO_MODER_MODER1 | GPIO_MODER_MODER2); //clears pins 1 2
    GPIOA->MODER |= (GPIO_MODER_MODER1 | GPIO_MODER_MODER2); //sets pins 1 and 2 as analog
}

//_________________ADC_CONFIGURATION_______________________________//
    /*PA1 --> ADC_IN1
      PA2 --> ADC_IN2*/

    //Need to setup DMA for transfer as well, since we are using two channels.
//________________________________________________________________//

//uint32_t x_pos = 0;
//uint32_t y_pos = 0; //Default values for the joystick position
    
void joystick_adc(void) {
    // RCC->APB2ENR |= RCC_APB2ENR_ADCEN; //Enable ADC1 clock
    // RCC->CR2 |= RCC_CR2_HSI14ON; //enabling the 14MHz high speed internal clock
    // while(!(RCC->CR2 & RCC_CR2_HSI14RDY)); //waiting for the clock) 
        
    // ADC1->CR |= ADC_CR_ADEN;

    // while(!(ADC1->ISR & ADC_ISR_ADRDY)); //waiting for the ADC to be ready

    // ADC1->CHSELR |= ADC_CHSELR_CHSEL1; //enabling the channel for ADC_IN1
    // ADC1->CHSELR |= ADC_CHSELR_CHSEL2; //enabling the channel for ADC_IN2
    // ADC1->CFGR1 &= ~ADC_CFGR1_SCANDIR; //setting the scan direction to be from channel 1 to channel 2
    // ADC1->CFGR1 &= ~ADC_CFGR1_CONT;      // Single conversion mode

    // while(!(ADC1->ISR & ADC_ISR_ADRDY)); //waiting for the ADC to be ready
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;     // Enable ADC1
    RCC->CR2     |= RCC_CR2_HSI14ON;        // Enable HSI14 (14 MHz)
    while (!(RCC->CR2 & RCC_CR2_HSI14RDY)); // Wait for HSI14 ready

    ADC1->CR &= ~ADC_CR_ADEN;               // Disable ADC before config
    while (ADC1->CR & ADC_CR_ADEN);         // Wait for disable

    ADC1->CHSELR = ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL2; // IN1 & IN2
    ADC1->CFGR1 &= ~ADC_CFGR1_SCANDIR;      // Upward scan: IN1 then IN2
    ADC1->CFGR1 &= ~ADC_CFGR1_CONT;         // Single conversion mode

    ADC1->CR |= ADC_CR_ADEN;                // Enable ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY));   // Wait for ready

}

// Boxcar average function to smooth out the values being read from the joystick
/*#define BCSIZE 32
int bcsum = 0;
int boxcar[BCSIZE];
int bcn = 0;*/

#define BCSIZE 32

// For y_pos (e.g., ADC_IN1)
int bcsum_y = 0;
int boxcar_y[BCSIZE];

// For x_pos (e.g., ADC_IN2)
int bcsum_x = 0;
int boxcar_x[BCSIZE];

// Shared boxcar index
int bcn = 0;

// Final output positions
uint32_t y_pos = 0;
uint32_t x_pos = 0;

float x_centered = 0.0f;
float y_centered = 0.0f;

void TIM2_IRQHandler () {
    TIM2->SR &= ~(TIM_SR_UIF);
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    ADC1->CR |= ADC_CR_ADSTART;
    
    while (!(ADC1->ISR & ADC_ISR_EOC));
    uint16_t y_raw = ADC1->DR;
    // 5. Wait for and read X-position (ADC_IN2)
    while (!(ADC1->ISR & ADC_ISR_EOC));
    uint16_t x_raw = ADC1->DR;

    // 6. Boxcar filtering for Y (vertical)
    bcsum_y -= boxcar_y[bcn];
    bcsum_y += boxcar_y[bcn] = y_raw;
    y_pos = bcsum_y / BCSIZE;

    // 7. Boxcar filtering for X (horizontal)
    bcsum_x -= boxcar_x[bcn];
    bcsum_x += boxcar_x[bcn] = x_raw;
    x_pos = bcsum_x / BCSIZE;

    // 8. Advance shared index
    bcn = (bcn + 1) % BCSIZE;

    x_centered = ((float)x_pos - 2048.0f) / 2048.0f;
    y_centered = ((float)y_pos - 2048.0f) / 2048.0f;

    if (x_centered > 1.0f) {
        x_centered = 1.0f;
    }
    if (x_centered < -1.0f) {
        x_centered = -1.0f;
    }
    if (y_centered > 1.0f) {
        y_centered = 1.0f;
    }
    if (y_centered < -1.0f) {
        y_centered = -1.0f;
    }

    // char buffer[128];
    // snprintf(buffer, sizeof(buffer), "X: %d\t Y: %d\r\n", x_pos, y_pos);
    // uart_send_string(buffer);

    // snprintf(buffer, sizeof(buffer), "X: %.2f\t Y: %.2f\r\n", x_centered, y_centered);
    // uart_send_string(buffer);
    // Send over UART
    //uart_send_float(x_centered);
}

void tim2_init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 480 - 1;     // Prescaler → 1 kHz timer clock
    TIM2->ARR = 500 - 1;       // 10 Hz trigger rate (every 100ms)
    TIM2->DIER |= TIM_DIER_UIE;  // Enable update interrupt
    TIM2->CR1 |= TIM_CR1_CEN;    // Start timer

    NVIC_EnableIRQ(TIM2_IRQn);
}



