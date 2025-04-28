#include "stm32f091xc.h"
#include "high_score.h"
#include "keypad.h"
#include "i2c.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>


/**
 * Since I'm not pushing my changes to main.c to prevent merge conflicts with anything you have,
 * I'm just going to briefly walk you through how to set up and run all of the leaderboard code 
 * that I've written. This should help you when you're working on the LED matrix display code, 
 * especially the integration with the leaderboard.
 * 
 * The two functions below allow you to test everything leaderboard-related:
 *  - set up the leaderboard and test reading and writing functionality
 *  - add to the leaderboard and write it back to EEPROM chip 
 *  
 * To get everything to compile and run properly, you will probably have to comment out a bunch of 
 * shit in other files. E.g. I had to CTRL+A, CTRL+'/' to comment out all of support.c and Zoey's 
 * mp3_to_dac.c because weird issues were causing compilation errors.
 */

// To run my I2C code, add the below lines in this order to your main function
/*
    int main() {
        enable_i2c_ports();
        enable_ports_keypad();

        init_i2c();
        init_usart5();
        enable_tty_interrupt();
        setbuf(stdin,0); // These turn off buffering
        setbuf(stdout,0);
        setbuf(stderr,0);

        // test functions:
        test_eeprom();
        test_add_to_leaderboard();
        
        return 0;
    }
 */



void test_add_to_leaderboard() {    
    High_score* leaderboard = load_high_scores_from_eeprom(); // important 
    printf("\nLeaderboard loaded from memory:\n");
    print_leaderboard(leaderboard);

    uint32_t player_score = 10000 + rand() % 1000; // random score for testing purposes

    printf("\nSimulating end of game. Player scored %lu points.\n", player_score);
    // the below 4 lines (minus print) will be necessary in main.c when combined with game
    setup_tim7(); // enables the timer to accept keypad inputs
    printf("Checking if player qualifies for leaderboard...\n"); // ignore
    leaderboard = update_leaderboard(leaderboard, player_score); // checks 
    disable_tim7();
        
    printf("\nUpdated Leaderboard:\n");
    print_leaderboard(leaderboard);

    // crucial line
    free_leaderboard(leaderboard); // free heap memory of leaderboard loaded from EEPROM
}

// if you want to test just reading w/o writing, comment out lines 74, 75, 83
void test_eeprom() {
    // create dummy leaderboard to test writing
    High_score* leaderboard = create_test_leaderboard();
    save_high_scores_to_eeprom(leaderboard);

    // read leaderboard from EEPROM chip's memory
    High_score* loaded_board = load_high_scores_from_eeprom();
    printf("\nLeaderboard loaded from memory:\n");
    print_leaderboard(loaded_board);

    free_leaderboard(loaded_board); // free heap memory of loaded board
    free_leaderboard(leaderboard); // free dummy leaderboard
}   