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

#include "i2c.h"
#include "high_score.h"

void update_leaderboard(High_score* head, uint32_t player_score) {
    High_score* curr = head;
    High_score* prev = NULL;
    while (curr != NULL) {
        if (player_score > curr->score) {
            add_new_high_score(prev, curr, player_score);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void add_new_high_score(High_score* prev, High_score* curr, uint32_t player_score) {
    High_score* new_node = malloc(sizeof(High_score));
    if (!new_node) return; // if True -> error malloc'ing
    char name_buf[4];
    get_player_name(name_buf);
    strncpy(new_node->name, name_buf, 4); // copy player name into name field
    new_node->score = player_score;
    new_node->next = curr;
    if (prev) {
        prev->next = new_node;
    }
    new_node->next = curr;
}

void get_player_name(char* buffer) {
    strncpy(buffer, "YOU", 4);
    // TBD
    // this will call the output of another function 
    // that I am not writing to get the characters
}

void print_leaderboard(High_score* head) {
    int rank = 1;
    while (head) {
        printf("%2d. %c%c%c - %ld\n", rank++,
               head->name[0], head->name[1], head->name[2],
               head->score);
        head = head->next;
    }
}

High_score* create_test_leaderboard() {
    High_score* head = NULL;
    High_score* tail = NULL;

    for (int i = 0; i < NUM_HIGH_SCORES; i++) {
        High_score* node = malloc(sizeof(High_score));
        if (!node) break; // error with malloc

        char ch = 'A' + i;
        node->name[0] = ch;
        node->name[1] = ch;
        node->name[2] = ch;
        node->score = 1000 * (i + 1);  // 1000 to 10000

        node->next = NULL;

        if (!head) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }

    return head;
}

void free_leaderboard(High_score* head) {
    while (head) {
        High_score* temp = head;
        head = head->next;
        free(temp);
    }
}
