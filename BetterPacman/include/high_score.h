/**
 * 
 * @file: high_score.h
 * @author: Alex Piet
 * 
 * @description: 
 *      Holds declarations for all read/write functions necessary
 *      to create a leaderboard for Team 10's "Better Pacman" game.
 * 
 */


#ifndef _HIGH_SCORE_H_
#define _HIGH_SCORE_H_ 1

/** Dictionary structured linked list for storing top 10 high scores */
typedef struct High_score {
    char name[4]; // player name, limit to 3 initials (+'\0') for now
    int score; // game score
    // int placement; // place on leaderboard - only top 10
    High_score* next; // store order in linked list, easy way to compare 
} High_score;

#define MAX_LINE_LENGTH 100


/**
 * @return Head of linked list of high score player data
 */
High_score* read_high_score_data();

/**
 * @param score Head of linked list containing (name, score) pairs
 * 
 * @return maybe return a bool or EXIT_SUCCESS to signify successful write?
 */
void write_high_score_data(High_score* scores);

/**
 * @param head Pointer to head of the leaderboard
 * @param name Player's initials to add to linked list
 * @param score Player's high score value
 */
void insert(High_score** head, const char* name, int score);


#endif /* _HIGH_SCORE_H_ */