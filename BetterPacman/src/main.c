#include "stm32f0xx.h"
#include "i2c.h"
#include "high_score.h"
#include "keypad.h"
#include "decl.h"
#include "gamelogic.h"
#include "led_matrix.h"

// void test_eeprom();
// void test_add_to_leaderboard();
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
    // enable_ports_keypad();
    // setup_tim7();     // <<< Add this to start TIM7!


    // before game
    High_score* leaderboard = load_high_scores_from_eeprom();

    // if (current_screen == WELCOME_SCREEN) {
    show_welcome_screen();
    for (int row = 0; row < 16; row++) {
        display_row(row);
    }
    // }
    if (current_screen == WELCOME_SCREEN) {
        welcome_timer++;
        if (welcome_timer > 80) {  // ~2 seconds depending on clock
            current_screen = NAME_ENTRY_SCREEN;
            need_screen_update = 1;  // <<< Force screen update when changing state
        }
    }

    // begin the game 
    start_game();
    uint32_t player_score = pacman.score;
    printf("Player score: %ld", player_score);

    // after game, check if player qualifies for leaderboard
    setup_tim7();
    update_leaderboard(leaderboard, player_score);
    disable_tim7();

    return EXIT_SUCCESS;
}