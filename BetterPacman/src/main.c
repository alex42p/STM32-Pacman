#include "stm32f0xx.h"
#include "i2c.h"
#include "high_score.h"
#include "keypad.h"
#include "decl.h"
#include "gamelogic.h"
#include "led_matrix.h"

extern void internal_clock();
extern volatile enum GameState current_screen;

int main(void) {
    internal_clock();   // Never comment this out!
    // Setup all peripherals
    enable_i2c_ports();
    enable_ports_keypad();
    init_i2c();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0); // These turn off buffering
    setbuf(stdout,0);
    setbuf(stderr,0);

    init_spi1_tft();
    lcd_init_sequence();
    init_usart5();      // Setup USART5 (TX = PC12, RX = PD2)
    joystickPin_config(); // Set joystick pins to analog
    joystick_adc();     // Setup ADC1 for reading joystick X and Y

    tim2_init();        // Start TIM2 periodic interrupt
    matrix_gpio_init();
    clear_screen();


    // before game
    High_score* leaderboard = load_high_scores_from_eeprom();
    // High_score* leaderboard = zero_out_leaderboard();

    for (welcome_timer = 0; welcome_timer < 250; welcome_timer++) {
        show_welcome_screen();
        for (int row = 0; row < 16; row++) {
            display_row(row);
        }
    }
    clear_screen();

    // begin the game 
    start_game(leaderboard);
    uint32_t player_score = pacman.score;
    printf("Player score: %ld", player_score);


    // after game, check if player qualifies for leaderboard
    clear_screen();
    setup_tim7();
    leaderboard = update_leaderboard(leaderboard, player_score);
    disable_tim7();

    clear_screen();


    while(1)
    {
        show_leader_board_screen(leaderboard, player_score);
        for (int row = 0; row < 16; row++) {
            display_row(row);
        }
    }

    return EXIT_SUCCESS;
}

// 1234 % 10 = 4
// 858 % 1000 = 858