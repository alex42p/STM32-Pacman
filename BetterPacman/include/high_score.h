/** Dictionary structured linked list for storing top 10 high scores */
typedef struct High_score {
    char name[4]; // player name, limit to 3 initials (+'\0') for now
    int score; // game score
    int placement; // place on leaderboard - only top 10
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
 * @param place Player's placement on the leaderboard
 */
void insert(High_score** head, const char* name, int score, int place);
