/**
 * 
 * @file: tftlcd.c
 * @author: Omkar Godke
 * 
 * @description: 
 *      Sets up the logic for the LCD screen to display the game.
 * 
 */

#include "stm32f0xx.h"
#include "tftlcd.h"

void nano_wait(unsigned int n);

// SPI1 Init with PB3 (SCK), PB5 (MOSI)
void init_spi1_tft(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // PA3 = RESET, PA4 = CS, PA6 = DC → GPIO output
    GPIOA->MODER &= ~((3 << (2*3)) | (3 << (2*15)) | (3 << (2*6)));
    GPIOA->MODER |=  ((1 << (2*3)) | (1 << (2*15)) | (1 << (2*6)));

    // PA5 = SCK, PA7 = MOSI → AF0
    GPIOA->MODER &= ~((3 << (2*5)) | (3 << (2*7)));
    GPIOA->MODER |=  ((2 << (2*5)) | (2 << (2*7))); // Alternate Function
    GPIOA->AFR[0] &= ~((0xF << (4*5)) | (0xF << (4*7))); // AF0 for SPI1

    // SPI1 config: Master mode, 8-bit, SSM
    SPI1->CR1 = 0;
    SPI1->CR1 |= SPI_CR1_MSTR;           // Master
    SPI1->CR1 &= ~(7 << 3);              // Baud = f_PCLK / 2
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR2 = SPI_CR2_FRXTH | (7 << SPI_CR2_DS_Pos); // 8-bit mode
    SPI1->CR1 |= SPI_CR1_SPE;            // Enable SPI
}
        


void spi_send(uint8_t byte) {
    while (!(SPI1->SR & SPI_SR_TXE));
    *((__IO uint8_t *)&SPI1->DR) = byte;
    while (SPI1->SR & SPI_SR_BSY);
}

void lcd_send_command(uint8_t cmd) {
    DC_CMD;
    CS_LOW;
    spi_send(cmd);
    CS_HIGH;
}

void lcd_send_data(uint8_t data) {
    DC_DATA;
    CS_LOW;
    spi_send(data);
    CS_HIGH;
}

void lcd_send_data16(uint16_t color) {
    lcd_send_data(color >> 8);
    lcd_send_data(color & 0xFF);
}

void lcd_reset(void) {
    RST_LOW;
    nano_wait(1000000);
    RST_HIGH;
    nano_wait(1000000);
}

void lcd_init_sequence(void) {
    lcd_reset();

    lcd_send_command(0x11); // Sleep out
    nano_wait(500000);      // Wait > 120ms

    lcd_send_command(0x3A); // Pixel format
    lcd_send_data(0x55);    // 16-bit color (RGB565)

    lcd_send_command(0x36); // Memory Access Control
    lcd_send_data(0x28);    // MX=1, BGR=1 (Landscape RGB) ← FIX 3 HERE

    lcd_send_command(0x29); // Display ON
    nano_wait(100000);      // Small delay
}


void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    lcd_send_command(0x2A); // Column addr
    lcd_send_data(x0 >> 8); lcd_send_data(x0 & 0xFF);
    lcd_send_data(x1 >> 8); lcd_send_data(x1 & 0xFF);

    lcd_send_command(0x2B); // Row addr
    lcd_send_data(y0 >> 8); lcd_send_data(y0 & 0xFF);
    lcd_send_data(y1 >> 8); lcd_send_data(y1 & 0xFF);

    lcd_send_command(0x2C); // Write to RAM
}

void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    lcd_set_window(x, y, x, y);
    lcd_send_data16(color);
}

void lcd_fill_screen(uint16_t color) {
    lcd_set_window(0, 0, 479, 319);  // Full 480x320
    for (int i = 0; i < 480 * 320; i++) {
        lcd_send_data16(color);
    }
}


void lcd_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg) {
    while (*str) {
        lcd_draw_char(x, y, *str, color, bg);
        x += 6; // 5 pixels wide + 1 pixel space
        str++;
    }
}

// #include "stm32f0xx.h"
// #include "tftlcd.h"

// void nano_wait(unsigned int n);

// // SPI1 Init with PB3 (SCK), PB5 (MOSI)
// void init_spi1_tft(void) {
//     RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
//     RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

//     // CS = PB8, RESET = PB11, DC = PB14 = outputs
//     GPIOB->MODER &= ~((3 << (2*8)) | (3 << (2*11)) | (3 << (2*14)));
//     GPIOB->MODER |=  ((1 << (2*8)) | (1 << (2*11)) | (1 << (2*14)));

//     // PB3 = SCK, PB5 = MOSI as AF0 (SPI1)
//     GPIOB->MODER &= ~((3 << (2*3)) | (3 << (2*5)));
//     GPIOB->MODER |=  ((2 << (2*3)) | (2 << (2*5)));
//     GPIOB->AFR[0] &= ~((0xF << (4*3)) | (0xF << (4*5)));  // AF0

//     // SPI1 config: master, 8-bit, slow baud, SSM
//     SPI1->CR1 = 0;
//     SPI1->CR1 |= SPI_CR1_MSTR;           // Master
//     SPI1->CR1 &= ~(7 << 3);  // Clear bits 5:3 (BR), sets BR = 000 = f_PCLK / 2
//     SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
//     SPI1->CR2 = SPI_CR2_FRXTH | (7 << SPI_CR2_DS_Pos); // 8-bit mode
//     SPI1->CR1 |= SPI_CR1_SPE;            // Enable SPI
// }

// void spi_send(uint8_t byte) {
//     while (!(SPI1->SR & SPI_SR_TXE));
//     *((__IO uint8_t *)&SPI1->DR) = byte;
//     while (SPI1->SR & SPI_SR_BSY);
// }

// void lcd_send_command(uint8_t cmd) {
//     DC_CMD;
//     CS_LOW;
//     spi_send(cmd);
//     CS_HIGH;
// }

// void lcd_send_data(uint8_t data) {
//     DC_DATA;
//     CS_LOW;
//     spi_send(data);
//     CS_HIGH;
// }

// void lcd_send_data16(uint16_t color) {
//     lcd_send_data(color >> 8);
//     lcd_send_data(color & 0xFF);
// }

// void lcd_reset(void) {
//     RST_LOW;
//     nano_wait(1000000);
//     RST_HIGH;
//     nano_wait(1000000);
// }

// void lcd_init_sequence(void) {
//     lcd_reset();

//     lcd_send_command(0x11); // Sleep out
//     nano_wait(500000);      // Wait > 120ms

//     lcd_send_command(0x3A); // Pixel format
//     lcd_send_data(0x55);    // 16-bit color (RGB565)

//     lcd_send_command(0x36); // Memory Access Control
//     lcd_send_data(0x28);    // MX=1, BGR=1 (Landscape RGB) ← FIX 3 HERE

//     lcd_send_command(0x29); // Display ON
//     nano_wait(100000);      // Small delay
// }


// void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
//     lcd_send_command(0x2A); // Column addr
//     lcd_send_data(x0 >> 8); lcd_send_data(x0 & 0xFF);
//     lcd_send_data(x1 >> 8); lcd_send_data(x1 & 0xFF);

//     lcd_send_command(0x2B); // Row addr
//     lcd_send_data(y0 >> 8); lcd_send_data(y0 & 0xFF);
//     lcd_send_data(y1 >> 8); lcd_send_data(y1 & 0xFF);

//     lcd_send_command(0x2C); // Write to RAM
// }

// void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
//     lcd_set_window(x, y, x, y);
//     lcd_send_data16(color);
// }

// void lcd_fill_screen(uint16_t color) {
//     lcd_set_window(0, 0, 479, 319);  // Full 480x320
//     for (int i = 0; i < 480 * 320; i++) {
//         lcd_send_data16(color);
//     }
// }


// void lcd_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg) {
//     while (*str) {
//         lcd_draw_char(x, y, *str, color, bg);
//         x += 6; // 5 pixels wide + 1 pixel space
//         str++;
//     }
// }