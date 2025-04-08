/**
 * 
 * @file: i2c.c
 * @author: Alex Piet
 * 
 * @description: 
 *      This file holds all of the code necessary to implement the read and 
 *      write functionality for the purpose of creating a leaderboard to be 
 *      displayed on the high score screen of Team 10's "Better Pacman" game.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "high_score.h"

/**
 * TODO:
 * - Finish implementing write function
 * - Test calling read/write functions from some main function
 */


High_score* read_high_score_data() {
    FILE* file = fopen("high_scores.txt", "r");
    if (file == NULL) {
        perror("Error opening file.");
        return 1; // exit_failure if file not found
    }

    High_score* head = NULL;
    char line[MAX_LINE_LENGTH];
    int placement = 1;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (placement > 10) break; // prevent reading >10 scores from text file
        // process each line and add (names, scores) to list of high_score structs
        line[strcspn(line, "\n")] = '\0';
        char name[4]; // 3 initials + '\0'
        int score;
        sscanf(line, "%3s,%d", name, &score); // read (3 chars, score)
        name[4] = '\0'; // add null terminator after reading player's initials
        insert(&head, name, score);
    }
    fclose(file);

    return head;
}

void write_high_score_data(High_score* scores) {
    FILE* file = fopen("high_scores.txt", "w");
    if (file == NULL) {
        perror("Error opening file.");
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

void insert(High_score** head, const char* name, int score) {
    High_score* new_node = malloc(sizeof(High_score));
    strcpy(new_node->name, name);
    new_node->score = score;
    // new_node->placement = 0; // update later
    new_node->next = NULL;

    // if empty list or new score > top score, update head of list
    if (*head == NULL || (*head)->score < score) {
        new_node->next = *head;
        *head = new_node;
    } else {
        High_score* curr = *head;
        // iterate to proper placement in high score list, then insert new score
        while(curr->next != NULL && curr->next->score >= score) {
            curr = curr->next;
        }
        new_node->next = curr->next;
        // new_node->placement = place;
        curr->next = new_node;
        
    }
}
