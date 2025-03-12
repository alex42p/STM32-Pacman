#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * TODO:
 * put struct and any/all #defines into related header file to clean up this src file 
 * finish writing read/write functions
 * test calling read/write functions from some main function
 * 
 */

/** Dictionary structured linked list for storing top 10 or so high scores */
typedef struct High_score {
    char name[3]; // limit to 3 initials(?)
    int score;
    High_score* next; // store order in linked list
} High_score;

#define MAX_LINE_LENGTH 100


/**
 * @return change return type to array of HS structs later
 */
void read_high_score_data() {
    FILE* file = fopen("high_scores.txt", "r");
    if (file == NULL) {
        perror("Error opening file.")
        return 1; // exit_failure if file not found
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // process each line and add (names, scores) to list of high_score structs
    }
    fclose(file);
    return;
}

/**
 * @param score Head of linked list containing (name, score) pairs
 * 
 * @return maybe return a bool or EXIT_SUCCESS to signify successful write?
 */
void write_high_score_data(High_score* scores) {
    FILE* file = fopen("high_scores.txt", "w");
    if (file == NULL) {
        perror("Error opening file.")
        return 1; // exit_failure if file not found
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // maybe change while() condition
        // write to each line of the HS file
    }
    fclose(file);
    return;
}