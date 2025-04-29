#include "stm32f0xx.h"
#include "decl.h"
#include <stdio.h>

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

char map[20][32] = {
    "################################",
    "##o...........##...........o##",
    "##.####.#####.##.#####.####.##",
    "##.####.#####.##.#####.####.##",
    "##.####.#####.##.#####.####.##",
    "##..........................##",
    "##.####.## ######## ##.####.##",
    "##.####.## ######## ##.####.##",
    "##......##          ##......##",
    "#######.######  ######.#######",
    "........##          ##........",
    "#######.##! ! ! ! !!##.#######",
    "##......##! ! ! ! !!##......##",
    "##.####.##!!!!!!!!!!##.####.##",
    "##.####.##############.####.##",
    "##..........................##",
    "##.####.##.########.##.####.##",
    "##.####.##.########.##.####.##",
    "##o.....##..........##.....o##",
    "##############################"
};

// global vars:
// Pac-Man struct initialization
Pacman pacman = { PACMAN_ORIG_COL, PACMAN_ORIG_ROW, PACMAN_ORIG_COL, PACMAN_ORIG_ROW, DIR_NONE, DIR_NONE, 0, 3 };

// ghosts structs intialization
Ghost blinky = {BLINKY_ORIG_COL, BLINKY_ORIG_ROW, BLINKY_ORIG_COL, BLINKY_ORIG_ROW, COLOR_BLINKY, DIR_NONE, true, false, 0};
Ghost pinky  = {PINKY_ORIG_COL, PINKY_ORIG_ROW, PINKY_ORIG_COL, PINKY_ORIG_ROW, COLOR_PINKY, DIR_NONE, true, false, 0};
Ghost inky   = {INKY_ORIG_COL, INKY_ORIG_ROW, INKY_ORIG_COL, INKY_ORIG_ROW, COLOR_INKY, DIR_NONE, true, false, 0};
Ghost clyde  = {CLYDE_ORIG_COL, CLYDE_ORIG_ROW, CLYDE_ORIG_COL, CLYDE_ORIG_ROW, COLOR_CLYDE, DIR_NONE, true, false, 0};

// joystick variables
extern volatile uint32_t y_pos;
extern volatile uint32_t x_pos;

// scared timers
int scared_timer = 0;
bool ghosts_scared = false;

// int main(void) {
//     internal_clock();
//     // initialize spi interfacing with TFT LCD 
//     init_spi1_tft();
//     lcd_init_sequence();
//     init_usart5();      // Setup USART5 (TX = PC12, RX = PD2)
//     joystickPin_config(); // Set joystick pins to analog
//     joystick_adc();     // Setup ADC1 for reading joystick X and Y
//     tim2_init();        // Start TIM2 periodic interrupt

//     start_game();

//     while (1);
// }

void start_game(void) {
    lcd_fill_screen(COLOR_BLACK);
    load_background();
    int frames = 0;
    bool game_won = false;
    while (pacman.lives > 0 && game_won == false) {
        nano_wait(20000000);
        frames++;
        if (frames >= PACMAN_SPEED * GHOST_REG_SPEED) {
            frames = 0;
        }
        if (ghosts_scared == true) {

            if (frames % GHOST_SCARED_SPEED == 0) {
                move_ghost(&blinky);
                move_ghost(&pinky);
                move_ghost(&inky);
                move_ghost(&clyde);
            }
            if (scared_timer > 0) {
                scared_timer--;
            } else {
                ghosts_scared = false;
                blinky.is_scared = false;
                pinky.is_scared = false;
                inky.is_scared = false;
                clyde.is_scared = false;
            }
        }
        else {
            if (frames % GHOST_REG_SPEED == 0) {
                move_ghost(&blinky);
                move_ghost(&pinky);
                move_ghost(&inky);
                move_ghost(&clyde);
            }
        }

        check_ghost_collision(&pacman, &blinky, &pinky, &inky, &clyde);

        // Map joystick position to movement direction
        if (x_pos < 1000) {
            pacman.desiredDir = DIR_LEFT;   
        } else if (x_pos > 3000) {
            pacman.desiredDir = DIR_RIGHT;
        } else if (y_pos < 1000) {
            pacman.desiredDir = DIR_DOWN;
        } else if (y_pos > 3000) {
            pacman.desiredDir = DIR_UP;
        }

        // Move Pac-Man based on the direction
        if (frames % PACMAN_SPEED == 0) {
            move_pacman(&pacman);
        }
        // char buffer[128];
        // snprintf(buffer, sizeof(buffer), "score: %ld\r\n", pacman.score);
        // uart_send_string(buffer);
        game_won = check_win();
    }
    if (game_won == true) {
        load_gamewon();
    } else {
        load_gameover();
    }
}
