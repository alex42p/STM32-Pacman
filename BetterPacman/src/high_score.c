#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "high_score.h"

/**
 * TODO:
 * [DONE] put struct and any/all #defines into related header file to clean up this src file 
 * [READ DONE] finish writing read/write functions
 * test calling read/write functions from some main function
 * 
 */


High_score* read_high_score_data() {
    FILE* file = fopen("high_scores.txt", "r");
    if (file == NULL) {
        perror("Error opening file.");
        return 1; // exit_failure if file not found
    }

    High_score* head = NULL;
    char line[MAX_LINE_LENGTH];
    int place = 1;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (place > 10) break; // prevent reading >10 scores from text file
        // process each line and add (names, scores) to list of high_score structs
        line[strcspn(line, "\n")] = '\0';
        char name[4]; // may need to change to 4 for '\0'?
        int score;
        sscanf(line, "%3s,%d", name, &score); // read (3 chars, score)
        insert(&head, name, score, place++);
    }
    fclose(file);

    return head;
}

void write_high_score_data(High_score* scores) {
    FILE* file = fopen("high_scores.txt", "w");
    if (file == NULL) {
        perror("Error opening file.");
        return 1; // exit_failure if file not found
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // maybe change while() condition
        // write to each line of the HS file
    }
    fclose(file);
    return;
}

void insert(High_score** head, const char* name, int score, int place) {
    High_score* new_node = malloc(sizeof(High_score));
    strcpy(new_node->name, name);
    new_node->score = score;
    new_node->placement = 0; // update later
    new_node->next = NULL;

    // if empty list or new score > top score, update head of list
    if (*head == NULL || (*head)->score < score) {
        new_node->next = *head;
        *head = new_node;
    } else {
        High_score* curr = *head;
        // iterate to proper placement in high score list, then insert new score
        while(curr->next != NULL && curr->next->score >= score) {
            curr = curr->next;
        }
        new_node->next = curr->next;
        new_node->placement = place;
        curr->next = new_node;
        
    }
}

void setup_i2c() {
    // transfer high score linked list data to the TFT-LCD display
    /** refer to i2c lab to see example of using i2c to transfer data */

    /** SETUP STEPS: */

    /** Enable Ports */
    // Enable the peripheral clock for the GPIOx bus for the pins for I2C1 SCL and SDA.
    // Configure the pins for the I2C1 alternate function.
    
    /** Init I2C */
    // enable the clock
    I2C1->CR1 &= ~I2C_CR1_PE; // clear enable bit while setting up i2c
    // Turn OFF the analog noise filter.
    // Turn ON the error interrupts enable.
    // Disable clock stretching.
    // To configure the I2C for 400 kHz "Fast Mode" transmission, we need to set TIMINGR register fields to specific values.
    // With internal_clock(), our STM32 clock frequency is 48 MHz.
    // The STM32 Reference Manual has a table that shows the values for the TIMINGR register for different examples of I2C speeds for different clock frequencies.
    // The timings settings assumes a I2CCLK of 8 MHz, so you will need to adjust the PRESC field to divide the clock down from 48 MHz accordingly.
    // You want to set the I2C speed to 400 kHz "Fast Mode", so you will need to adjust the SCLL, SCLH, SDADEL and SCLDEL respectively.
    // 100 kHz "Standard Mode" would work too. Always check the datasheet for the device you are communicating with to see what speeds it supports.
    // Configure I2C1 to be in 7-bit addressing mode, and not 10-bit addressing mode.
    // Finally, enable the I2C1 peripheral.


    
    I2C1->CR1 |= I2C_CR1_PE; // enable I2C after setting up data transfer process
}

void i2c_start(uint8_t targadr, int size, bool dir) {
    // 0. Take current contents of CR2 register. 
    uint32_t tmpreg = I2C1->CR2;

    // 1. Clear the following bits in the tmpreg: SADD, NBYTES, RD_WRN, START, STOP

    // 2. Set read/write direction in tmpreg.

    // 3. Set the target's address in SADD (shift targadr left by 1 bit) and the data size.
    tmpreg |= ((targadr<<1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);

    // 4. Set the START bit.
    tmpreg |= I2C_CR2_START;

    // 5. Start the conversion by writing the modified value back to the CR2 register.
    I2C1->CR2 = tmpreg;
}

void i2c_stop() {
    // 0. If a STOP bit has already been sent, return from the function.
    // Check the I2C1 ISR register for the corresponding bit.

    // 1. Set the STOP bit in the CR2 register.

    // 2. Wait until STOPF flag is reset by checking the same flag in ISR.

    // 3. Clear the STOPF flag by writing 1 to the corresponding bit in the ICR.
}

void i2c_idle() {
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

void i2c_recvdata() {
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

int i2c_checknack() {
    return 1;
}

void i2c_clearnack() {

}