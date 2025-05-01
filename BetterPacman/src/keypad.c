/**
 * 
 * @file: keypad.c
 * @authors: Alex Piet
 * 
 * @description: 
 *      This file holds the necessary code to use the keypad for
 *      a player to input their name to be saved to the leaderboard.
 * 
 */

#include "keypad.h"
#include "led_matrix.h"

uint8_t col = 0;
char disp[4]; // 3 letters + '\0'
volatile int name_entry_done = 0; // flag to check if player entered name
char name[4] = {'A', 'A', 'A', '\0'}; // 3 letters + '\0'
int letter_idx = 0; // index of name entry
uint8_t hist[16]; // 16 history bytes.  Each byte represents the last 8 samples of a button.
char queue[2];  // A two-entry queue of button press/release events.
int qin;        // Which queue entry is next for input
int qout;       // Which queue entry is next for output
char keymap_arr[17] = "DCBA#9630852*741";



void enable_ports_keypad(){
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~0xffff;
    GPIOC->MODER |= 0x55 << (4*2);
    GPIOC->PUPDR &= ~0xff;
    GPIOC->PUPDR |= 0x55;
}


void setup_tim7() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 48000 - 1;
    TIM7->ARR = 40 - 1;
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= (1 << TIM7_IRQn);
    TIM7->CR1 |= TIM_CR1_CEN;
}

void disable_tim7() {
    TIM7->CR1 &= ~TIM_CR1_CEN;
}

void TIM7_IRQHandler() {
    TIM7->SR &= ~TIM_SR_UIF; // clear interrupt flag

    for (int row = 0; row < 16; row++) {
        display_row(row);
    }
    
    int rows = read_rows();

    if (rows != 0) {
        char key = rows_to_key(rows);
        if (key != 0) 
            handle_key(key);
            clear_screen();
            show_name_entry_screen();

            
    }

    col++;
    if (col > 7) {
        col = 0;
    }
    drive_column(col);
}

void drive_column(int c)
{
    GPIOC->BSRR = 0xf00000 | ~(1 << (c + 4));
}

int read_rows()
{
    return (~GPIOC->IDR) & 0xf;
}

char rows_to_key(int rows) {
    // Note `rows` will be a 4 bit value from reading the IDR register of the row pins of the keypad (comes from read_rows call)
    // compute the offset of the button being pressed right now from `rows` and `col` (start checking from the lowest row)
    int row_num;
    // lower row values take precedence if multiple buttons are pressed
    if (rows & (1 << 0))        row_num = 0;
    else if (rows & (1 << 1))   row_num = 1;
    else if (rows & (1 << 2))   row_num = 2;
    else                        row_num = 3;
    // lookup `c` in the `keymap_arr` indexed by the offsets
    char c = keymap_arr[4 * col + row_num];
    return c;
}

void handle_key(char key) {
    // printf("Key pressed: %d \n", key); // uncomment for debugging
    // if key = 'A'/'B', +/- char
    // if key = '#', lock in char
    switch(key) {
        case 'A':   
            name[letter_idx]++;
            if (name[letter_idx] > 'Z') { // wrap around
                name[letter_idx] = 'A';
            }
            break;
        case 'B':   
            name[letter_idx]--;
            if (name[letter_idx] < 'A') { // wrap around
                name[letter_idx] = 'Z';
            }
            break; 
        case '#':   
            printf("Letter idx: %d \n", letter_idx);
            letter_idx++; // move to next letter
            if (letter_idx == 3) {
                name[3] = '\0';
                name_entry_done = 1;
            }
            break;
    }
}

char get_key_event(void) {
    for(;;) {
        asm volatile ("wfi");   // wait for an interrupt
        if (queue[qout] != 0)
            break;
    }
    return pop_queue();
}

void push_queue(int n) {
    queue[qin] = n;
    qin ^= 1;
}

char pop_queue() {
    char tmp = queue[qout];
    queue[qout] = 0;
    qout ^= 1;
    return tmp;
}

void update_history(int c, int rows)
{
    // We used to make students do this in assembly language.
    for(int i = 0; i < 4; i++) {
        hist[4*c+i] = (hist[4*c+i]<<1) + ((rows>>i)&1);
        if (hist[4*c+i] == 0x01)
            push_queue(0x80 | keymap_arr[4*c+i]);
        if (hist[4*c+i] == 0xfe)
            push_queue(keymap_arr[4*c+i]);
    }
}

char get_keypress() {
    char event;
    for(;;) {
        // Wait for every button event...
        event = get_key_event();
        // ...but ignore if it's a release.
        if (event & 0x80)
            break;
    }
    return event & 0x7f;
}

// this function will need to be updated later when display is properly set up
void clear_display() {
    for (int i = 0; i < 4; i++) {
        disp[i] = ' ';
    }
}