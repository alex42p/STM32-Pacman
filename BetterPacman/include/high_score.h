/**
 * 
 * @file: high_score.h
 * @author: Alex Piet
 * 
 * @description: 
 *      Holds declarations for all read/write and helper functions 
 *      necessary to create and store a leaderboard for Team 10's 
 *      "Better Pacman" game.
 * 
 */

#ifndef _HIGH_SCORE_H_
#define _HIGH_SCORE_H_ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "i2c.h"

/** Dictionary structured linked list for storing top 10 high scores */
typedef struct High_score {
    char name[4]; // Player's name
    uint32_t score; // Game score
    struct High_score* next; // Pointer to next score in leaderboard
} High_score;

#define MAX_LINE_LENGTH 100
#define NUM_HIGH_SCORES 10

/**
 * @brief Copy the player's name from button input into a buffer
 * 
 * @param buffer Save the player's input name
 */
void get_player_name(char* buffer);

/**
 * @brief Inserts game score into a new spot on the leaderboard
 * 
 * @param prev Score above player's new high score
 * @param curr Score below player's new high score
 * @param player_score Player's game score
 */
void add_new_high_score(High_score* prev, High_score* curr, uint32_t player_score);

/**
 * @brief Checks if the game score is higher than any existing
 *        leaderboard scores, and inserts into list if it is via
 *        a call to @function add_new_high_score
 *  
 * @param head Head node of leaderboard
 * @param player_score Player's game score
 */
void update_leaderboard(High_score* head, uint32_t player_score);

/**
 * @brief Creates a dummy leaderboard for debugging purposes
 * 
 * @returns Head node of test leaderboard
 */
High_score* create_test_leaderboard();

/**
 * @brief Frees all malloc'd memory from the heap
 * 
 * @param head Head node of leaderboard
 */
void free_leaderboard(High_score* head);

/**
 * @brief Prints the leaderboard to stdout
 * 
 * @param head Head node of leaderboard
 */
void print_leaderboard(High_score* head);

#endif /* _HIGH_SCORE_H_ */