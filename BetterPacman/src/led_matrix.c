#include "stm32f0xx.h"
#include <string.h>
#include "keypad.h"
#include "led_matrix.h"

volatile enum GameState current_screen = WELCOME_SCREEN;
// Timing variables
volatile int welcome_timer = 0;
volatile int need_screen_update = 0;

const uint8_t font5x7[96][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x00,0x00,0x5F,0x00,0x00}, // '!'
    {0x00,0x07,0x00,0x07,0x00}, // '"'
    {0x14,0x7F,0x14,0x7F,0x14}, // '#'
    {0x24,0x2A,0x7F,0x2A,0x12}, // '$'
    {0x23,0x13,0x08,0x64,0x62}, // '%'
    {0x36,0x49,0x55,0x22,0x50}, // '&'
    {0x00,0x05,0x03,0x00,0x00}, // '''
    {0x00,0x1C,0x22,0x41,0x00}, // '('
    {0x00,0x41,0x22,0x1C,0x00}, // ')'
    {0x14,0x08,0x3E,0x08,0x14}, // '*'
    {0x08,0x08,0x3E,0x08,0x08}, // '+'
    {0x00,0x50,0x30,0x00,0x00}, // ','
    {0x08,0x08,0x08,0x08,0x08}, // '-'
    {0x00,0x60,0x60,0x00,0x00}, // '.'
    {0x20,0x10,0x08,0x04,0x02}, // '/'
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'
    {0x00,0x42,0x7F,0x40,0x00}, // '1'
    {0x42,0x61,0x51,0x49,0x46}, // '2'
    {0x21,0x41,0x45,0x4B,0x31}, // '3'
    {0x18,0x14,0x12,0x7F,0x10}, // '4'
    {0x27,0x45,0x45,0x45,0x39}, // '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'
    {0x01,0x71,0x09,0x05,0x03}, // '7'
    {0x36,0x49,0x49,0x49,0x36}, // '8'
    {0x06,0x49,0x49,0x29,0x1E}, // '9'
    {0x00,0x36,0x36,0x00,0x00}, // ':'
    {0x00,0x56,0x36,0x00,0x00}, // ';'
    {0x08,0x14,0x22,0x41,0x00}, // '<'
    {0x14,0x14,0x14,0x14,0x14}, // '='
    {0x00,0x41,0x22,0x14,0x08}, // '>'
    {0x02,0x01,0x51,0x09,0x06}, // '?'
    {0x32,0x49,0x79,0x41,0x3E}, // '@'
    {0x7E,0x11,0x11,0x11,0x7E}, // 'A'
    {0x7F,0x49,0x49,0x49,0x36}, // 'B'
    {0x3E,0x41,0x41,0x41,0x22}, // 'C'
    {0x7F,0x41,0x41,0x22,0x1C}, // 'D'
    {0x7F,0x49,0x49,0x49,0x41}, // 'E'
    {0x7F,0x09,0x09,0x09,0x01}, // 'F'
    {0x3E,0x41,0x49,0x49,0x7A}, // 'G'
    {0x7F,0x08,0x08,0x08,0x7F}, // 'H'
    {0x00,0x41,0x7F,0x41,0x00}, // 'I'
    {0x20,0x40,0x41,0x3F,0x01}, // 'J'
    {0x7F,0x08,0x14,0x22,0x41}, // 'K'
    {0x7F,0x40,0x40,0x40,0x40}, // 'L'
    {0x7F,0x02,0x0C,0x02,0x7F}, // 'M'
    {0x7F,0x04,0x08,0x10,0x7F}, // 'N'
    {0x3E,0x41,0x41,0x41,0x3E}, // 'O'
    {0x7F,0x09,0x09,0x09,0x06}, // 'P'
    {0x3E,0x41,0x51,0x21,0x5E}, // 'Q'
    {0x7F,0x09,0x19,0x29,0x46}, // 'R'
    {0x46,0x49,0x49,0x49,0x31}, // 'S'
    {0x01,0x01,0x7F,0x01,0x01}, // 'T'
    {0x3F,0x40,0x40,0x40,0x3F}, // 'U'
    {0x1F,0x20,0x40,0x20,0x1F}, // 'V'
    {0x7F,0x20,0x18,0x20,0x7F}, // 'W'
    {0x63,0x14,0x08,0x14,0x63}, // 'X'
    {0x03,0x04,0x78,0x04,0x03}, // 'Y'
    {0x61,0x51,0x49,0x45,0x43}, // 'Z'
};

const uint8_t font3x5[96][3] = {
    {0x00, 0x00, 0x00}, // ' '
    {0x00,0x17,0x00}, // '!'
    {0x03,0x00,0x03}, // '"'
    {0x1F,0x0A,0x1F}, // '#'
    {0x0A,0x1F,0x05}, // '$'
    {0x19,0x04,0x13}, // '%'
    {0x0A,0x15,0x1A}, // '&'
    {0x00,0x03,0x00}, // '''
    {0x00,0x0E,0x11}, // '('
    {0x11,0x0E,0x00}, // ')'
    {0x05,0x02,0x05}, // '*'
    {0x04,0x0E,0x04}, // '+'
    {0x10,0x08,0x00}, // ','
    {0x04,0x04,0x04}, // '-'
    {0x00,0x10,0x00}, // '.'
    {0x18,0x04,0x03}, // '/'
    {0x1F, 0x11, 0x1F}, // '0'
    {0x12, 0x1F, 0x10}, // '1'
    {0x1D, 0x15, 0x17}, // '2'
    {0x15, 0x15, 0x1F}, // '3'
    {0x07, 0x04, 0x1F}, // '4'
    {0x17, 0x15, 0x1D}, // '5'
    {0x1F, 0x15, 0x1D}, // '6'
    {0x01, 0x01, 0x1F}, // '7'
    {0x1F, 0x15, 0x1F}, // '8'
    {0x17, 0x15, 0x1F}, // '9'
    {0x00,0x0A,0x00}, // ':'
    {0x10,0x0A,0x00}, // ';'
    {0x04,0x0A,0x11}, // '<'
    {0x0A,0x0A,0x0A}, // '='
    {0x11,0x0A,0x04}, // '>'
    {0x01,0x15,0x02}, // '?'
    {0x0E,0x15,0x16}, // '@'
    {0x1E,0x05,0x1E}, // 'A'
    {0x1F,0x15,0x0A}, // 'B'
    {0x0E,0x11,0x11}, // 'C'
    {0x1F,0x11,0x0E}, // 'D'
    {0x1F,0x15,0x11}, // 'E'
    {0x1F,0x05,0x01}, // 'F'
    {0x0E,0x11,0x1D}, // 'G'
    {0x1F,0x04,0x1F}, // 'H'
    {0x11,0x1F,0x11}, // 'I'
    {0x08,0x10,0x0F}, // 'J'
    {0x1F,0x04,0x1B}, // 'K'
    {0x1F,0x10,0x10}, // 'L'
    {0x1F,0x06,0x1F}, // 'M'
    {0x1F,0x0E,0x1F}, // 'N'
    {0x0E,0x11,0x0E}, // 'O'
    {0x1F,0x05,0x02}, // 'P'
    {0x0E,0x11,0x1E}, // 'Q'
    {0x1F,0x0D,0x16}, // 'R'
    {0x12,0x15,0x09}, // 'S'
    {0x01,0x1F,0x01}, // 'T'
    {0x0F,0x10,0x1F}, // 'U'
    {0x07,0x18,0x07}, // 'V'
    {0x1F,0x0C,0x1F}, // 'W'
    {0x1B,0x04,0x1B}, // 'X'
    {0x03,0x1C,0x03}, // 'Y'
    {0x19,0x15,0x13}  // 'Z'
};

void matrix_gpio_init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  // Enable GPIOB
    GPIOB->MODER |= 0x55555555;           // All PBx pins to output
    GPIOB->OTYPER |= 0;
    GPIOB->OSPEEDR |= 0xFFFFFFFF;         // High speed
}

void latch_data(void) {
    GPIOB->BSRR = (1 << LAT_PIN);
    GPIOB->BSRR = (1 << (LAT_PIN + 16));
}

void enable_output(void) {
    GPIOB->BRR = (1 << OE_PIN);
}

void disable_output(void) {
    GPIOB->BSRR = (1 << OE_PIN);
}

void set_row_address(uint8_t row) {
    uint32_t set_mask = 0;
    uint32_t reset_mask = 0;

    if (row & 0x01) set_mask |= (1 << A_PIN); else reset_mask |= (1 << A_PIN);
    if (row & 0x02) set_mask |= (1 << B_PIN); else reset_mask |= (1 << B_PIN);
    if (row & 0x04) set_mask |= (1 << C_PIN); else reset_mask |= (1 << C_PIN);
    if (row & 0x08) set_mask |= (1 << D_PIN); else reset_mask |= (1 << D_PIN);

    GPIOB->BSRR = (reset_mask << 16) | set_mask;
}

void clear_screen(void) {
    memset(framebuffer, 0, sizeof(framebuffer));
}

static inline void fast_pulse_clk(void) {
    GPIOB->BSRR = (1 << CLK_PIN);           // Set CLK high
    GPIOB->BSRR = (1 << (CLK_PIN + 16));     // Set CLK low
}

void display_row(uint8_t row) {
    disable_output();
    set_row_address(row);

    for (int col = 0; col < WIDTH; col++) {
        uint8_t top_pixel = framebuffer[row][col];
        uint8_t bot_pixel = framebuffer[row + 16][col];

        uint32_t set_mask = 0;
        uint32_t reset_mask = 0;

        if (top_pixel & 0x01) set_mask |= (1 << R1_PIN); else reset_mask |= (1 << R1_PIN);
        if (top_pixel & 0x02) set_mask |= (1 << G1_PIN); else reset_mask |= (1 << G1_PIN);
        if (top_pixel & 0x04) set_mask |= (1 << B1_PIN); else reset_mask |= (1 << B1_PIN);

        if (bot_pixel & 0x01) set_mask |= (1 << R2_PIN); else reset_mask |= (1 << R2_PIN);
        if (bot_pixel & 0x02) set_mask |= (1 << G2_PIN); else reset_mask |= (1 << G2_PIN);
        if (bot_pixel & 0x04) set_mask |= (1 << B2_PIN); else reset_mask |= (1 << B2_PIN);

        GPIOB->BSRR = (reset_mask << 16) | set_mask;
        fast_pulse_clk();
    }

    latch_data();
    enable_output();
    for (volatile int i = 0; i < BRIGHTNESS_DELAY; i++);
    disable_output();
}

void draw_char(char c, int x, int y, uint8_t color) {
    if (c < 32 || c > 126) return;  // Ignore unsupported
    const uint8_t *glyph = font5x7[c - 32];

    for (int col = 0; col < 5; col++) {
        uint8_t bits = glyph[col];
        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    framebuffer[py][px] = color;
                }
            }
        }
    }
}

void draw_string(const char *str, int x, int y, uint8_t color) {
    while (*str) {
        draw_char(*str, x, y, color);
        x += 6;  // 5 pixels + 1 space
        str++;
    }
}

void show_welcome_screen(void) {
    draw_string("WELCOME!", 8, 2, 0b111);    // White text
    draw_string("PLAYER", 16, 18, 0b101);     // Pink text
}

void show_name_entry_screen(void) {
    // Draw the current letter big
    char current_letter[2] = { name[letter_idx], '\0' }; // Single character string
    draw_string(current_letter, 28, 5, 0b111);  // Centered big letter

    // Draw the typed name so far
    draw_string(name, 5, 20, 0b011);             // Bottom line for name
}

void draw_small_char(char c, int x, int y, uint8_t color) {
    if (c < 32 || c > 127) return;
    const uint8_t *glyph = font3x5[c - 32];

    for (int col = 0; col < 3; col++) {
        uint8_t bits = glyph[col];
        for (int row = 0; row < 5; row++) {
            if (bits & (1 << row)) {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    framebuffer[py][px] = color;
                }
            }
        }
    }
}

void draw_small_string(const char *str, int x, int y, uint8_t color) {
    while (*str) {
        draw_small_char(*str, x, y, color);
        if (*str == ' '){
            x += 1;
        }
        else{
        x += 4;  // 3 pixels + 1 space
        }
        str++;
    }
}

void draw_small_number(int number, int x_start, int y, uint8_t color) {
    char buffer[12];
    int i = 0;

    if (number == 0) {
        buffer[i++] = '0';
    }

    while (number > 0) {
        buffer[i++] = (number % 10) + '0';
        number /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        draw_small_char(buffer[j], x_start, y, color);
        x_start += 4;
    }
}

void draw_number(int number, int x_start, int y, uint8_t color) {
    char buffer[12];
    int i = 0;

    if (number == 0) {
        buffer[i++] = '0';
    }

    while (number > 0) {
        buffer[i++] = (number % 10) + '0';
        number /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        draw_char(buffer[j], x_start, y, color);
        x_start += 6;
    }
}

void show_current_score_screen(u_int32_t score) {
    draw_small_string("SCORE", 41, 8, 0b111); // White text
    draw_small_number(score, 39, 15, 0b1011);
}

// void show_person_board(const char *string, u_int32_t score) {
//     draw_small_string(*string, 5, 5, 0b111); // White text
//     draw_small_number(score, 20, 5, 0b101);
// }

void show_leader_board_screen(High_score* head, uint32_t score) {
    show_current_score_screen(score);
    draw_small_string("HIGH SCORES", 1, 2, 0b111); // White text
    int y = 8;
    uint32_t i = 1;
    while (head) {
        draw_small_number(i++, 1, y, 0b111);
        draw_small_string(head->name, 6, y, 0b111); // White text
        draw_small_number(head->score, 19, y, 0b101);
        head = head->next;
        y += 6; // Move down for the next entry
    }
}