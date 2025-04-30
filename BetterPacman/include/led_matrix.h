#ifndef _LED_MATRIX_H
#define _LED_MATRIX_H 1

#include "high_score.h"

extern volatile int name_entry_done;
extern char name[4];
extern int letter_idx;

// Game states
enum GameState {
    WELCOME_SCREEN,
    NAME_ENTRY_SCREEN
};

extern volatile enum GameState current_screen;

// Timing variables
extern volatile int welcome_timer;

extern volatile int need_screen_update;

extern void setup_tim3(void);
extern void matrix_gpio_init(void);
extern void display_row(uint8_t row);
extern void draw_char(char c, int x, int y, uint8_t color);
extern void draw_string(const char *str, int x, int y, uint8_t color);
extern void show_welcome_screen(void);
extern void clear_screen(void);


#define BRIGHTNESS_DELAY 50  // Play with this value!

extern const uint8_t font5x7[96][5]; 

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





#endif /* _LED_MATRIX_H */