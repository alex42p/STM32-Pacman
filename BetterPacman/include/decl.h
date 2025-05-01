#include<stdint.h>
#include<stdbool.h>
#include<math.h>
#include "gamelogic.h"
#include "high_score.h"

extern struct Ghost g;
void nano_wait(unsigned int n);

// clock.c
void internal_clock(void);

// tftlcd.c
void init_spi1_tft(void);
void lcd_send_command(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_send_data16(uint16_t color);
void lcd_init_sequence(void);
void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void lcd_fill_screen(uint16_t color);
void lcd_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg);
void lcd_draw_char(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg);

// pacman.c
void draw_pacman(Pacman *p);
void move_pacman(Pacman *p);
void check_ghost_collision(Pacman *p, Ghost *g1, Ghost *g2, Ghost *g3, Ghost *g4);

// ghost.c
void draw_ghost(Ghost *g);
void move_ghost_up(Ghost *g);
void move_ghost_down(Ghost *g);
void move_ghost_left(Ghost *g);
void move_ghost_right(Ghost *g);
bool is_intersection(uint8_t col, uint8_t row);
Direction random_direction();
void move_ghost(Ghost *g);
void release_ghost(Ghost *g);
void reset_ghost(Ghost *g);


// map.c
void load_background(void);
void erase_tile(uint8_t col, uint8_t row);
bool is_wall(uint8_t col, uint8_t row);
void draw_pellet(uint8_t col, uint8_t row);
void load_gameover(void);
void draw_power_pellet(uint8_t col, uint8_t row);
void load_gamewon(void);
bool check_win(void);


// main.c
void start_game(High_score* leaderboard, int level);

//joystick_pos.c
void joystickPin_config(void);
void joystick_adc(void);
void TIM2_IRQHandler(void);
void tim2_init(void);
void upate_joystick(void);

// joystick_test.c
void init_usart5(void);
void uart_send_char(char c);
void uart_send_string(const char *s);
void uart_send_float(float value);