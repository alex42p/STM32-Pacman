#include <stdint.h>
#include <stdio.h>
#include "stm32f0xx.h"
#include audio_data.h
#include audio_data.c

/*
We will need to convert the mp3 file to a PCM in order to be used bty the STM. It is recommended that we use our computers to convert the file
instead of the STM as it requires a lot of power.

1. Initialize DAC3
2. Initialize TIM6
3. Initialize DMA
    -Optional looping to add sound after trigger event

sounds:

1. Pacman dying
2. background music
3. high score sound
4. bite sound

Mechanism to switch between sounds
*/

int main(void){

    enable_port_DAC();
    setup_dac();
    setup_dma();
    enable_dma();

    while(1){}

}

void enable_port_DAC(){

    //enable clock + ports depending on timer and port used
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set PA4 to Analog mode for DAC output
    GPIOA->MODER &= ~GPIO_MODER_MODE4;  // Clear the mode for PA4
    GPIOA->MODER |= GPIO_MODER_MODE4_1;  // Set PA4 to analog mode

}

void setup_dma(){


    RCC->AHBENR |= RCC_AHBENR_DMA1EN; //enabling the clock
    DMA1_Channel5->CCR &= ~DMA_CCR_EN; //disabling the channel
    DMA1_Channel5->CMAR = (uint32_t) audio_data; //assigning CMAR to the address of msg -> will have to change this to the PCM array
    DMA1_Channel5->CPAR = (uint32_t) &DMA->DHR8R1; //CPAR is assigned to the address of the port B's ODR
    DMA1_Channel5->CNDTR = 8; //CNDTR set to 8 LEDs
    DMA1_Channel5->CCR |= DMA_CCR_DIR; //enabling memory to peripheral mode
    DMA1_Channel5->CCR |= DMA_CCR_MINC; //MINC increment for every CMAR transfer
    DMA1_Channel5->CCR |= 0x0; //MSIZE to 8-bit
    DMA1_Channel5->CCR |= 0x0; //PSIZE to 8-bit
    DMA1_Channel5->CCR |= DMA_CCR_CIRC; //enabling circular operation 
    

}

void enable_dma(void) {
    
    DMA1_Channel5->CCR |= DMA_CCR_EN; //enable channel 5

}

void setup_dac() {

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enabling port A
    GPIOA->MODER &= ~(0x300); //resetting and setting port 4 as analog mode
    GPIOA->MODER |= 0x300;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN; //enabling rcc clock for dac
    DAC->CR |= DAC_CR_TEN1;
    //DAC->CR |= DAC_CR_TSEL1;
    DAC->CR |= DAC_CR_EN1;

}
/*
void init_tim15(void) { //timer to handle DMA transfer

    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 4800 - 1;
    TIM15->ARR = 10 - 1;
    TIM15->DIER |= TIM_DIER_UDE;
    TIM15->CR1 |= TIM_CR1_CEN;

} */

