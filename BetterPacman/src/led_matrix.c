#include "stm32f0xx.h"
#include <string.h>
#include "keypad.h"

volatile int need_screen_update = 0;
extern void setup_tim3(void);
extern void matrix_gpio_init(void);
extern void display_row(uint8_t row);
extern void draw_char(char c, int x, int y, uint8_t color);
extern void draw_string(const char *str, int x, int y, uint8_t color);
extern void show_welcome_screen(void);


#define BRIGHTNESS_DELAY 50  // Play with this value!

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

#define WIDTH 64
#define HEIGHT 32

uint8_t framebuffer[HEIGHT][WIDTH];

#define R1_PIN 0
#define G1_PIN 1
#define B1_PIN 2
#define R2_PIN 3
#define G2_PIN 4
#define B2_PIN 5
#define A_PIN 6
#define B_PIN 7
#define C_PIN 8
#define D_PIN 9
#define CLK_PIN 10
#define LAT_PIN 11
#define OE_PIN 12

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

//extern void

extern volatile int name_entry_done;
extern char name[4];
extern int letter_idx;

// Game states
enum GameState {
    WELCOME_SCREEN,
    NAME_ENTRY_SCREEN
};

volatile enum GameState current_screen = WELCOME_SCREEN;

// Timing variables
volatile int welcome_timer = 0;

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


int main(void) {
    gpio_init();
    clear_screen();
    enable_ports_keypad();
    setup_tim7();     // <<< Add this to start TIM7!
    //keypad_test_loop();
    //setup_tim3();     // <<< Add this to start TIM3!
    //need_screen_update = 1; // Force initial screen update

    while (1) {

        need_screen_update = 1; // Set the flag to update the screen

        if (current_screen == WELCOME_SCREEN) {
            welcome_timer++;
            if (welcome_timer > 80) {  // ~2 seconds depending on clock
                current_screen = NAME_ENTRY_SCREEN;
                need_screen_update = 1;  // <<< Force screen update when changing state
            }
        }

        if (need_screen_update) {
            clear_screen();
    
            if (current_screen == WELCOME_SCREEN) {
                show_welcome_screen();
            }
            else if (current_screen == NAME_ENTRY_SCREEN) {
                show_name_entry_screen();
            }
    
            for (int row = 0; row < 16; row++) {
                display_row(row);
            }
    
            need_screen_update = 0; // clear flag
        }
      }
}