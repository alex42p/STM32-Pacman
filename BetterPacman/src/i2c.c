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

#include "i2c.h"
#include "high_score.h"


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
        // data[i] &= I2C_TXDR_TXDATA; // mask data[i] with 0xFF to ensure only 8 bits long
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
//===========================================================================

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

void save_high_scores_to_eeprom(High_score* head) {
    int i = 0;
    High_score* curr = head;
    while (curr != NULL) {
        uint16_t addr = i * 32;
        uint8_t buffer[7];

        // copy name over
        buffer[0] = curr->name[0];
        buffer[1] = curr->name[1];
        buffer[2] = curr->name[2];
        
        // copy score over
        buffer[3] = (curr->score >> 0) & 0xFF;
        buffer[4] = (curr->score >> 8) & 0xFF;
        buffer[5] = (curr->score >> 16) & 0xFF;
        buffer[6] = (curr->score >> 24) & 0xFF;

        uint8_t len = sizeof(buffer);
        eeprom_write(addr, (char*)buffer, len);
        
        // DEBUG PRINT - DO NOT REMOVE OR CODE WILL BREAK!!!!!
        fprintf(stdout, "Writing to EEPROM @ 0x%04X: Name='%c%c%c', Score=%lu => Bytes: [%02X %02X %02X %02X %02X %02X %02X]\n",
            addr,
            curr->name[0], curr->name[1], curr->name[2],
            curr->score, // (uint32_t)curr->score,
            buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
        //
        addr += len;
        curr = curr->next;
        i++;
    }
}

High_score* load_high_scores_from_eeprom() {
    High_score* head = NULL;
    High_score* tail = NULL;
    uint8_t buffer[7];    

    for (int i = 0; i < NUM_HIGH_SCORES; i++) {
        uint16_t addr = i * EEPROM_PAGE_SIZE;        
        eeprom_read(addr, (char*)buffer, sizeof(buffer));


        High_score* node = malloc(sizeof(High_score));
        if (!node) {
            printf("Malloc error with entry %d: %s\n", i, strerror(errno));
            break; // error allocating memory
        }

        // copy name
        node->name[0] = buffer[0];
        node->name[1] = buffer[1];
        node->name[2] = buffer[2];
        node->name[3] = '\0';
        
        // copy score
        node->score  = ((uint32_t)buffer[3] << 0);
        node->score |= ((uint32_t)buffer[4] << 8);
        node->score |= ((uint32_t)buffer[5] << 16);
        node->score |= ((uint32_t)buffer[6] << 24);
        
        node->next = NULL;

        // if empty list, set head to new node
        if (head == NULL) {
            head = node;
            tail = node;
        } else { // otherwise append to end
            tail->next = node;
            tail = node;
        }
        addr += sizeof(buffer);
    }
    return head;
}
