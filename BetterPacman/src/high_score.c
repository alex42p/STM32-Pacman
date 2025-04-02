#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "high_score.h"

/**
 * TODO:
 * [DONE] put struct and any/all #defines into related header file to clean up this src file 
 * [READ DONE] finish writing read/write functions
 * test calling read/write functions from some main function
 * 
 */


High_score* read_high_score_data() {
    FILE* file = fopen("high_scores.txt", "r");
    if (file == NULL) {
        perror("Error opening file.");
        return 1; // exit_failure if file not found
    }

    High_score* head = NULL;
    char line[MAX_LINE_LENGTH];
    int place = 1;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (place > 10) break; // prevent reading >10 scores from text file
        // process each line and add (names, scores) to list of high_score structs
        line[strcspn(line, "\n")] = '\0';
        char name[3];
        int score;
        sscanf(line, "%3s,%d", name, &score); // read (3 chars, score)
        insert(&head, name, score, place++);
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

void insert(High_score** head, const char* name, int score, int place) {
    High_score* new_node = malloc(sizeof(High_score));
    strcpy(new_node->name, name);
    new_node->score = score;
    new_node->placement = 0; // update later
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
        new_node->placement = place;
        curr->next = new_node;
        
    }
}

void setup_i2c() {
    I2C1->CR1 &= ~I2C_CR1_PE; // clear enable bit while setting up i2c
    // figure out the rest of the necessary bits to enable to 
    // transfer high score linked list data to the TFT-LCD display
    /** refer to  */
    I2C1->CR1 |= I2C_CR1_PE; // enable I2C after setting up data transfer process
}
