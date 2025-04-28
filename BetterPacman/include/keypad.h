#ifndef KEYPAD_H
#define KEYPAD_H 1

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f091xc.h"
#include "high_score.h"
#include "i2c.h"

extern volatile int name_entry_done;
extern char name[4];
extern int letter_idx;
extern char keymap_arr[17];

void handle_key(char key);
void enable_ports_keypad();
void setup_tim7();
void disable_tim7();
void drive_column(int c);
int read_rows();
char rows_to_key(int rows);
void clear_display();
void show_char();
char pop_queue();
char get_key_event();
void push_queue();
void update_history(int c, int rows);
char get_keypress();


#endif
