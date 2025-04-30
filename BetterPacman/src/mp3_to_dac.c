#include <stdint.h>
#include <stdio.h>
#include "stm32f0xx.h"
#include "audio_data.h"
// #include "audio_data.c"

int idx = 0;

/*
We will need to convert the mp3 file to a PCM in order to be used bty the STM. It is recommended that we use our computers to convert the file
instead of the STM as it requires a lot of power.

1. Initialize DAC3
2. Initialize TIM6
3. Initialize DMA
    -Optional looping to add sound after trigger event

sounds:
Mechanism to switch between sounds

For the main function:
- Call play_audio(); and the the DMA/DAC/Timer should be set, and will turn off at the end of the sound
- This function should be called every time Pacman eats a pellet


int main(void){

    enable_port_DAC();
    setup_dac();
    setup_dma();
    enable_dma();

    while(1){}

} */

void enable_port_DAC(){

    //enable clock + ports depending on timer and port used
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set PA4 to Analog mode for DAC output
    GPIOA->MODER &= ~GPIO_MODER_MODER4;  // Clear the mode for PA4
    GPIOA->MODER |= GPIO_MODER_MODER4;  // Set PA4 to analog mode

}

void setup_dma(){


    RCC->AHBENR |= RCC_AHBENR_DMA1EN; //enabling the clock
    DMA1_Channel5->CCR &= ~DMA_CCR_EN; //disabling the channel
    DMA1_Channel5->CMAR = (uint32_t) audio_data; //assigning CMAR to the address of msg -> will have to change this to the PCM array
    DMA1_Channel5->CPAR = (uint32_t) &DAC->DHR12R1; //CPAR is assigned to the address of the port B's ODR -- try changing this to 12R array
    DMA1_Channel5->CNDTR = audio_data_len; //CNDTR set to 8 LEDs
    DMA1_Channel5->CCR |= DMA_CCR_DIR; //enabling memory to peripheral mode
    DMA1_Channel5->CCR |= DMA_CCR_MINC; //MINC increment for every CMAR transfer
    DMA1_Channel5->CCR &= ~DMA_CCR_MSIZE; //MSIZE to 8-bit
    DMA1_Channel5->CCR &= ~DMA_CCR_MSIZE; //PSIZE to 8-bit
    //DMA1_Channel5->CCR |= DMA_CCR_CIRC; //enabling circular operation 
    //DMA1_Channel5->CCR |= DMA_CCR_TCIE;
    

}

void enable_dma(void) {
    
    DMA1_Channel5->CCR |= DMA_CCR_EN; //enable channel 5

}

void setup_dac() {

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enabling port A
    GPIOA->MODER &= ~GPIO_MODER_MODER4; //resetting and setting port 4 as analog mode
    GPIOA->MODER |= GPIO_MODER_MODER4;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN; //enabling rcc clock for dac
    DAC->CR |= DAC_CR_TEN1;
    DAC->CR &= ~DAC_CR_TSEL1; //timer 6 trigger event
    DAC->CR |= DAC_CR_EN1;

}

void init_tim6(void) {

    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->PSC = 500 - 1;
    TIM6->ARR = 5 - 1;
    TIM6->CR2 |= TIM_CR2_MMS_1;
    NVIC->ISER[0] = 1 << TIM6_IRQn;
    TIM6->CR1 |= TIM_CR1_CEN;

}

void TIM6_IRQHandler(){

    TIM6->SR &= ~TIM_SR_UIF;

    if(idx >= audio_data_len){
        DAC->CR &= ~DAC_CR_EN1;
        TIM6->CR1 &= ~TIM_CR1_CEN;
        DMA1_Channel5->CCR &= ~DMA_CCR_EN;
    }

    // if(idx >= audio_data_len){
    //     idx = 0;
    // }

    uint8_t sample = audio_data[idx];
    int value = (sample + 128) * 16;
    DAC->DHR12R1 = value;

    idx++;
}

void play_audio(){

    idx = 0;

    enable_port_DAC();
    setup_dac();
    setup_dma();
    enable_dma();
    init_tim6();
}

