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
 * - Add necessary additions to display leaderboard somewhere 
 */



//===========================================================================
// Configure SDA and SCL.
//===========================================================================
void enable_i2c_ports(void) {
    /** PA9: SCL (Clock), PA10: SDA (Data Line) */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // enable port A clk
    GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1; // set PA9-10 to AF mode
    GPIOA->AFR[1] |= (4 << 4) | (4 << 8); // set PA9-10 to AF4
}

//===========================================================================
// Configure I2C1.
//===========================================================================
void init_i2c(void) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // enable I2C1 clock
    I2C1->CR1 &= ~I2C_CR1_PE; // clear enable bit while setting up I2C1
    I2C1->CR1 |= I2C_CR1_ANFOFF; // Turn OFF the analog noise filter.
    I2C1->CR1 |= I2C_CR1_ERRIE; // Turn ON the error interrupts enable.
    I2C1->CR1 |= I2C_CR1_NOSTRETCH; // Disable clock stretching.

    I2C1->TIMINGR |= (2 - 1) << I2C_TIMINGR_PRESC_Pos; // 48MHz = 6 * 8MHz
    // Set SCLL, SCLH, SDADEL and SCLDEL - TABLE 83
    I2C1->TIMINGR |= ((3-1) << I2C_TIMINGR_SCLDEL_Pos) 
                    |((3-1) << I2C_TIMINGR_SDADEL_Pos)
                    |((3-1) << I2C_TIMINGR_SCLH_Pos)
                    |((9-1) << I2C_TIMINGR_SCLL_Pos); 

    I2C1->CR2 &= ~I2C_CR2_ADD10; // set 7bit addressing mode
    I2C1->CR2 &= ~I2C_CR2_AUTOEND; // probably useless 
    I2C1->CR1 |= I2C_CR1_PE; // enable I2C after initialization is complete
}

//===========================================================================
// Send a START bit. write=0, read=1
//===========================================================================
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir) { 
    // 0. Take current contents of CR2 register. 
    uint32_t tmpreg = I2C1->CR2;

    // 1. Clear the following bits in the tmpreg: SADD, NBYTES, RD_WRN, START, STOP
    tmpreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);

    // 2. Set read/write direction in tmpreg.
    // tmpreg |= (dir & 1) << I2C_CR2_RD_WRN_Pos; // DOUBLE CHECK THIS LINE
    if (dir) {
         tmpreg |= I2C_CR2_RD_WRN; // set 1 for read
    } else {
         tmpreg &= ~I2C_CR2_RD_WRN; // set 0 for write
    }
    // targadr = (targadr << 1);

    // 3. Set the target's address in SADD (shift targadr left by 1 bit) and the data size.
    tmpreg |= ((targadr<<1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);

    // 4. Set the START bit.
    tmpreg |= I2C_CR2_START;

    // 5. Start the conversion by writing the modified value back to the CR2 register.
    I2C1->CR2 = tmpreg;
}

//===========================================================================
// Send a STOP bit.
//===========================================================================
void i2c_stop(void) {
    // 0. If a STOP bit has already been sent, return from the function. Check the I2C1 ISR register for the corresponding bit.
    if (I2C1->ISR & I2C_ISR_STOPF) return;
    // 1. Set the STOP bit in the CR2 register.
    I2C1->CR2 |= I2C_CR2_STOP;
    // 2. Wait until STOPF flag is reset by checking the same flag in ISR.
    while (!(I2C1->ISR & I2C_ISR_STOPF));
    // 3. Clear the STOPF flag by writing 1 to the corresponding bit in the ICR.
    I2C1->ICR |= I2C_ICR_STOPCF;
}

//===========================================================================
// Wait until the I2C bus is not busy. (One-liner!)
//===========================================================================
void i2c_waitidle(void) {
    // int temp = 1;
    // while (I2C1->ISR & I2C_ISR_BUSY) {
    //     temp = I2C1->ISR & I2C_ISR_BUSY;
    //     if (temp == 0) break;
    // } // while BUSY, wait
    while (I2C1->ISR & I2C_ISR_BUSY); // while BUSY, wait
}

//===========================================================================
// Send each char in data[size] to the I2C bus at targadr.
//===========================================================================
int i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size) {
    i2c_waitidle(); // wait until I2C is idle
    i2c_start(targadr, size, 0); // START write(0)

    for (uint8_t i = 0; i <= size - 1; i++) {
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
        data[i] &= I2C_TXDR_TXDATA; // mask data[i] with 0xFF to ensure only 8 bits long
        I2C1->TXDR = data[i];
    }

    // loop until these flags are both not set 
    while (!(I2C1->ISR & I2C_ISR_TC) && !(I2C1->ISR & I2C_ISR_NACKF));

    if (!(I2C1->ISR & I2C_ISR_NACKF) == 0) return -1; // target device did not acknowledge the data
    i2c_stop();
    return EXIT_SUCCESS; // 0
}

//===========================================================================
// Receive size chars from the I2C bus at targadr and store in data[size].
//===========================================================================
int i2c_recvdata(uint8_t targadr, void *data, uint8_t size) {
    i2c_waitidle();
    i2c_start(targadr, size, 1); // START read(1)
    
    for (u_int8_t i=0; i <= size-1; i++) {
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
        ((uint8_t*)data)[i] = I2C1->RXDR & I2C_RXDR_RXDATA;
    }
    i2c_stop();
    return EXIT_SUCCESS;
}

//===========================================================================
// Clear the NACK bit. (One-liner!)
//===========================================================================
void i2c_clearnack(void) {
    I2C1->ICR |= I2C_ICR_NACKCF;
}

//===========================================================================
// Check the NACK bit. (One-liner!)
//===========================================================================
int i2c_checknack(void) {
    return (I2C1->ISR & I2C_ISR_NACKF);
}

//===========================================================================
// EEPROM functions
// We'll give these so you don't have to figure out how to write to the EEPROM.
// These can differ by device.

#define EEPROM_ADDR 0x57

void eeprom_write(uint16_t loc, const char* data, uint8_t len) {
    uint8_t bytes[34];
    bytes[0] = loc>>8;
    bytes[1] = loc&0xFF;
    for(int i = 0; i<len; i++){
        bytes[i+2] = data[i];
    }
    i2c_senddata(EEPROM_ADDR, bytes, len+2);
}

void eeprom_read(uint16_t loc, char data[], uint8_t len) {
    uint8_t bytes[2];
    bytes[0] = loc>>8;
    bytes[1] = loc&0xFF;
    i2c_senddata(EEPROM_ADDR, bytes, 2);
    i2c_recvdata(EEPROM_ADDR, data, len);
}

//===========================================================================
// Copy in code from Lab 7 - USART
//===========================================================================

#include "fifo.h"
#include "tty.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

//===========================================================================
// init_usart5
//===========================================================================
void init_usart5() {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;
    // Do all the steps necessary to configure pin PC12 to be routed to USART5_TX.
    GPIOC->MODER &= ~(GPIO_MODER_MODER12); // clear PC12
    GPIOC->MODER |= GPIO_MODER_MODER12_1; // set PC12 to AF (0b10)
    GPIOC->AFR[1] |= 0x2 << GPIO_AFRH_AFSEL12_Pos; // set PC12 to AF2
    // Do all the steps necessary to configure pin PD2 to be routed to USART5_RX.
    GPIOD->MODER &= ~(GPIO_MODER_MODER2); // clear PD2
    GPIOD->MODER |= GPIO_MODER_MODER2_1; // set PD2 to AF (0b10)
    GPIOD->AFR[0] |= 0x2 << GPIO_AFRL_AFSEL2_Pos; // set PD2 to AF2

    RCC->APB1ENR |= RCC_APB1ENR_USART5EN; // enable USART5 clock
    USART5->CR1 &= ~USART_CR1_UE; // turn off enable bit
    USART5->CR1 &= ~USART_CR1_M; // set word size to 8 bits
    USART5->CR2 &= ~USART_CR2_STOP; // set 1 stop bit
    USART5->CR1 &= ~USART_CR1_PCE; // no parity bit
    USART5->CR1 &= ~USART_CR1_OVER8; // set 16b oversampling
    USART5->BRR = 0x1A1; //48 * 1000000 / 115200; // divide system clock rate by BR=115200  == 0x1A1 (417)
    USART5->CR1 |= USART_CR1_TE | USART_CR1_RE; // enable Tx/Rx
    USART5->CR1 |= USART_CR1_UE; // set enable bit
    // waits to return from function until T/R EACK bits are set
    while (!(USART5->ISR & USART_ISR_TEACK)); // !(1&1) breaks loop
    while (!(USART5->ISR & USART_ISR_REACK)); // this indicates that the USART is ready to transmit and receive.
}

//===========================================================================
// enable_tty_interrupt
//===========================================================================
void enable_tty_interrupt(void) {
    // TODO
    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2->CSELR |= DMA2_CSELR_CH2_USART5_RX;
    DMA2_Channel2->CCR &= ~DMA_CCR_EN;  // First make sure DMA is turned off
    USART5->CR1 |= USART_CR1_RXNEIE; // enable RXNE interrupt
    USART5->CR3 |= USART_CR3_DMAR;   // Enable DMA for USART5 Rx
    NVIC->ISER[0] |= (1 << USART3_8_IRQn); // enable NVIC for USART5

    // The DMA channel 2 configuration goes here
    DMA2_Channel2->CMAR = (uint32_t) &(serfifo);
    DMA2_Channel2->CPAR = (uint32_t) &(USART5->RDR);
    DMA2_Channel2->CNDTR = FIFOSIZE;
    DMA2_Channel2->CCR &= ~(DMA_CCR_DIR); // read from peripheral to memory
    DMA2_Channel2->CCR &= ~(DMA_CCR_TEIE | DMA_CCR_HTIE); // disable half-transfer and total complete interrupts
    DMA2_Channel2->CCR &= ~(DMA_CCR_MSIZE | DMA_CCR_PSIZE); // set M/P-size ti 8b
    DMA2_Channel2->CCR &= ~(DMA_CCR_PINC); // disable PINC
    DMA2_Channel2->CCR |= DMA_CCR_MINC; // set MINC to increment CMAR
    DMA2_Channel2->CCR |= DMA_CCR_CIRC; // set circular mode
    DMA2_Channel2->CCR &= ~DMA_CCR_MEM2MEM; // explicitly disable MEM2MEM

    DMA2_Channel2->CCR |= DMA_CCR_PL; // set priority level to Very High
    NVIC_SetPriority(USART3_8_IRQn, 0); // set USART5 interrupt to highest priority
    
    DMA2_Channel2->CCR |= DMA_CCR_EN; // enable DMA again after configuration completes
}

//===========================================================================
// interrupt_getchar
//===========================================================================
char interrupt_getchar() {
    // TODO
    // Wait for a newline to complete the buffer.
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi"); // wait for an interrupt
    }
    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}

//===========================================================================
// __io_putchar
//===========================================================================
int __io_putchar(int c) {
    // TODO copy from STEP2
    if (c == '\n') {
        while(!(USART5->ISR & USART_ISR_TXE)); // wait for Tx reg to be empty
        USART5->TDR = '\r'; // write carriage return before newline
    }
    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

//===========================================================================
// __io_getchar
//===========================================================================
int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    return interrupt_getchar();
}

//===========================================================================
// IRQHandler for USART5
//===========================================================================
void USART3_8_IRQHandler(void) {
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo))
            insert_echo_char(serfifo[seroffset]);
        seroffset = (seroffset + 1) % sizeof serfifo;
    }
}

//===========================================================================
// Command functions for the shell
// These are the functions that are called when a command is entered, in 
// order to parse the command being entered, split into argc/argv calling 
// convention, and then execute the command by calling the respective 
// function.  We implement "write" and "read" for you to easily test your 
// EEPROM functions from a terminal.
//===========================================================================

void write(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: write <addr> <data>\n");
        printf("Ensure the address is a hexadecimal number.  No need to include 0x.\n");
        return;
    }
    uint32_t addr = atoi(argv[1]); 
    // concatenate all args from argv[2], until empty string is found, to a string
    char data[32] = "";
    int i = 0;
    int j = 2;
    while (strcmp(argv[j], "") != 0 && i < 32) {
        for (char c = argv[j][0]; c != '\0'; c = *(++argv[j])) {
            data[i++] = c;
        }
        if (strcmp(argv[j+1], "") != 0)
            data[i++] = ' ';
        else {
            data[i + 1] = '\0';
        }
        j++;
    }
    // ensure addr is a multiple of 32
    if ((addr % 10) != 0) {
        printf("Address 0x%ld is not evenly divisible by 32.  Your address must be a hexadecimal value.\n", addr);
        return;
    }
    int msglen = strlen(data) + 1;
    if (msglen > 32) {
        printf("Data is too long. Max length is 32.\n");
        return;
    }
    printf("Writing to address 0x%ld: %s\n", addr, data);
    eeprom_write(addr, data, msglen);
}

void read(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: read <addr>\n");
        printf("Ensure the address is a hexadecimal number.  No need to include 0x.\n");
        return;
    }
    uint32_t addr = atoi(argv[1]); 
    char data[32];
    // ensure addr is a multiple of 32
    if ((addr % 10) != 0) {
        printf("Address 0x%ld is not evenly divisible by 32.  Your address must be a hexadecimal value.\n", addr);
        return;
    }
    eeprom_read(addr, data, 32);
    printf("String at address 0x%ld: %s\n", addr, data);
}

struct commands_t {
    const char *cmd;
    void      (*fn)(int argc, char *argv[]);
};

struct commands_t cmds[] = {
    { "write", write },
    { "read", read }
};

void exec(int argc, char *argv[])
{
    for(int i=0; i<sizeof cmds/sizeof cmds[0]; i++)
        if (strcmp(cmds[i].cmd, argv[0]) == 0) {
            cmds[i].fn(argc, argv);
            return;
        }
    printf("%s: No such command.\n", argv[0]);
}

void parse_command(char *c)
{
    char *argv[20];
    int argc=0;
    int skipspace=1;
    for(; *c; c++) {
        if (skipspace) {
            if (*c != ' ' && *c != '\t') {
                argv[argc++] = c;
                skipspace = 0;
            }
        } else {
            if (*c == ' ' || *c == '\t') {
                *c = '\0';
                skipspace=1;
            }
        }
    }
    if (argc > 0) {
        argv[argc] = "";
        exec(argc, argv);
    }
}

//===========================================================================
// main()
//===========================================================================
/*
int main() {
    internal_clock();
    
    // I2C specific
    enable_ports();
    init_i2c();

    // If you don't want to deal with the command shell, you can 
    // comment out all code below and call 
    // eeprom_read/eeprom_write directly.
    init_usart5();
    enable_tty_interrupt();
    // These turn off buffering.
    setbuf(stdin,0); 
    setbuf(stdout,0);
    setbuf(stderr,0);
    // uint8_t dataaaaa = {1,2,34};
    //i2c_start(0x56, 0, 0);
    //i2c_stop();
    // while(1){
    //     printf("Reached first print in main.\n");
    //     i2c_senddata(0x56, dataaaaa, 2);
    //     printf("Reached second print in main.\n");
    // }
    
    printf("I2C Command Shell\n");
    printf("This is a simple shell that allows you to write to or read from the I2C EEPROM at %d.\n", EEPROM_ADDR);
    for(;;) {
        printf("\n> ");
        char line[100];
        fgets(line, 99, stdin);
        line[99] = '\0';
        int len = strlen(line);
        if (line[len-1] == '\n')
            line[len-1] = '\0';
        parse_command(line);
    }
}


*/