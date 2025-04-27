/**
 * 
 * @file: keypad.c
 * @authors: Zoey Halligan, Alex Piet
 * 
 * @description: 
 *      This file holds the necessary code to use the keypad for
 *      a player to input their name to be saved to the leaderboard.
 * 
 */

#include "stm32f0xx.h"
#include <stdint.h>
#include "high_score.h"

uint8_t col = 0;
// char current_char = 'A';
char disp[4];
char name[4] = {'A', 'A', 'A', '\0'}; // 3 letters + '\0'
int current_letter_index = 0; // index of name array


void enable_ports_keypad(){
    //assuming we want to use the same ports for the keypad--to be confirmed
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~(0x0000ffff);
    GPIOC->MODER |= 0x00005500;
    GPIOC->PUPDR &= ~(0x000000ff);
    GPIOC->PUPDR |= 0x000000aa;
}

void drive_column(int c)
{
    GPIOC->BSRR = 0xf00000 | ~(1 << (c + 4));
}

int read_rows() {
    return (~GPIOC->IDR) & 0xf;
}

//rows_to_key will read whether A or B was pressed, returns +1 or -1 respectively

char rows_to_key(int rows) {
    char c;
    if (rows & 0x1){
        return 'A'; // increment letter
    } else if (rows & 0x2){
        return 'B'; // decrement letter
    } else if (rows & 0x8) {
        if (col==2) {
            return '#'; // lock in letter
        }
    }
    return 0; // no valid entry 
}

//handler function to take in the value from rows_to_key and update the display variable accordingly

void handle_key(char key) {
    if (key == 'A') {
        // Rotate forward
        name[current_letter_index]++;
        if (name[current_letter_index] > 'Z') name[current_letter_index] = 'A';
    } else if (key == 'B') {
        // Rotate backward
        name[current_letter_index]--;
        if (name[current_letter_index] < 'A') name[current_letter_index] = 'Z';
    } else if (key == '#') {
        current_letter_index++; // move to next letter
        if (current_letter_index >= 3) {
            name[3] = '\0';
            // load the leaderboard and check if it needs 
            High_score* leaderboard = load_high_scores_from_eeprom();
            update_leaderboard(leaderboard, score);

            clear_display(); // wipe the characters off of the display
        }
    }

  // Update display (e.g., leftmost column)
    disp[current_letter_index] = name[current_letter_index];
}

// update to clear display
void clear_display() {
    for (int i = 0; i < 4; i++) {
        disp[i] = ' ';
    }
}

//will need to include a init_tim and handler to update the display as the display variable is changed

void TIM7_IRQHandler(){

    TIM7->SR &= ~TIM_SR_UIF;

    int rows = read_rows();
    if (rows != 0) {
        int key = rows_to_key(rows);
        handle_key(key);
    }

    char c  = disp[col];

    show_char(col, c);
    col++;
    if (col > 7){
        col = 0;
    }
    drive_column(col);
}

//initialize 
void setup_tim7() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 48000 - 1;
    TIM7->ARR = 50 - 1;
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] = (1 << 18);
    TIM7->CR1 |= TIM_CR1_CEN;
}

//write display
