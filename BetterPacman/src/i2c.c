/**
 * 
 * @file: i2c.c
 * @author: Alex Piet
 * 
 * @description: 
 *      This file holds all of the code necessary to implement the I2C 
 *      protocol to transfer data sequentially to and from the TFT-LCD
 *      for the purpose of displaying a high score leaderboard screen 
 *      for Team 10's "Better Pacman" game.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "i2c.h"

/**
 * TODO:
 * - Finish implementing code from i2c lab (setup, start, stop, idle, recvdata, checknack, clearnack)
 * - Add necessary additions to display leaderboard to the TFT-LCD 
 */


void enable_i2c_ports() {
    /** PA9: SCL (Clock), PA10: SDA (Data Line) */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // enable port A clk
    GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1; // set PA9-10 to AF mode
    GPIOA->AFR[1] |= (4 << 4) | (4 < 8); // set PA9-10 to AF4
}

void setup_i2c() {
    // transfer high score linked list data to the TFT-LCD display

    /** SETUP STEPS: */
    
    /** Init I2C */
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // enable i2c1 clock
    I2C1->CR1 &= ~I2C_CR1_PE; // clear enable bit while setting up i2c1
    I2C1->CR1 |= I2C_CR1_ANFOFF; // Turn OFF the analog noise filter.
    I2C1->CR1 |= I2C_CR1_ERRIE; // Turn ON the error interrupts enable.
    I2C1->CR1 |= I2C_CR1_NOSTRETCH; // Disable clock stretching.

    /** Relevant FRM pages: 615, 630, 670 */

    // To configure the I2C for 400 kHz "Fast Mode" transmission, we need to set TIMINGR register fields to specific values.
    // With internal_clock(), our STM32 clock frequency is 48 MHz.
    // The STM32 Reference Manual has a table that shows the values for the TIMINGR register for different examples of I2C speeds for different clock frequencies.
    // The timings settings assumes a I2CCLK of 8 MHz, so you will need to adjust the PRESC field to divide the clock down from 48 MHz accordingly.
    // You want to set the I2C speed to 400 kHz "Fast Mode", so you will need to adjust the SCLL, SCLH, SDADEL and SCLDEL respectively.
    // 100 kHz "Standard Mode" would work too. Always check the datasheet for the device you are communicating with to see what speeds it supports.
    // Configure I2C1 to be in 7-bit addressing mode, and not 10-bit addressing mode.
    // Finally, enable the I2C1 peripheral.
    /* Enable "Fast Mode" Transmission */
    I2C1->TIMINGR |= 0;

    I2C1->CR1 |= I2C_CR1_PE; // enable I2C after initialization is complete
}

void i2c_start(uint8_t targadr, int size, bool dir) { /** TODO: Complete this function */
    // 0. Take current contents of CR2 register. 
    uint32_t tmpreg = I2C1->CR2;

    // 1. Clear the following bits in the tmpreg: SADD, NBYTES, RD_WRN, START, STOP
    tmpreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
    // 2. Set read/write direction in tmpreg.
    
    // 3. Set the target's address in SADD (shift targadr left by 1 bit) and the data size.
    tmpreg |= ((targadr<<1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);

    // 4. Set the START bit.
    tmpreg |= I2C_CR2_START;

    // 5. Start the conversion by writing the modified value back to the CR2 register.
    I2C1->CR2 = tmpreg;
}

void i2c_stop() { /** TODO: Check that this function is complete */
    // 0. If a STOP bit has already been sent, return from the function. Check the I2C1 ISR register for the corresponding bit.
    if (I2C1->ISR & I2C_ISR_STOPF) return;
    // 1. Set the STOP bit in the CR2 register.
    I2C1->CR2 |= I2C_CR2_STOP;
    // 2. Wait until STOPF flag is reset by checking the same flag in ISR.
    while (!(I2C1->ISR & I2C_ISR_STOPF)); // ';' does nothing until loop returns false to break
    // 3. Clear the STOPF flag by writing 1 to the corresponding bit in the ICR.
    I2C1->ICR |= I2C_ICR_STOPCF;
}

void i2c_idle() { /** TODO: Rest of the function */
    int count = 0;
    while ((I2C1->ISR & I2C_ISR_TXIS) == 0) {
        count += 1;
        if (count > 1000000)
            return -1;
        if (i2c_checknack()) {
            i2c_clearnack();
            i2c_stop();
            return -1;
        }
    }
}

int i2c_recvdata() { /** TODO: Rest of the function */
    int count = 0;
    while ((I2C1->ISR & I2C_ISR_RXNE) == 0) {
        count += 1;
        if (count > 1000000)
            return -1;
        if (i2c_checknack()) {
            i2c_clearnack();
            i2c_stop();
            return -1;
        }
    }
}

int i2c_checknack() { /** TODO: Rest of the function */
    return 1;
}

int i2c_clearnack() { /** TODO: Rest of the function */

}