#ifndef _GAME_LOGIC_H_
#define _GAME_LOGIC_H_ 1
#include<stdbool.h>
// colors
#define COLOR_RED    0xF800
#define COLOR_GREEN  0x07E0
#define COLOR_BLUE   0x001F
#define COLOR_BLACK  0x0000
#define COLOR_WHITE  0xFFFF
#define COLOR_PACMAN  0xFFE0  // yellow
#define COLOR_BLINKY  0xF800  // Red
#define COLOR_PINKY   0xF81F  // Pink
#define COLOR_INKY    0x07FF  // Cyan
#define COLOR_CLYDE   0xFEA0  // Orange

// definining original positions for the sprites
#define BLINKY_ORIG_COL 14
#define BLINKY_ORIG_ROW 10
#define PINKY_ORIG_COL 15
#define PINKY_ORIG_ROW 11
#define INKY_ORIG_COL 14
#define INKY_ORIG_ROW 12
#define CLYDE_ORIG_COL 15
#define CLYDE_ORIG_ROW 13
#define PACMAN_ORIG_COL 14
#define PACMAN_ORIG_ROW 15

# define PACMAN_SPEED 7
# define GHOST_REG_SPEED 9
# define GHOST_SCARED_SPEED 11

// map specific constants and variables
#define TILE_SIZE 16
extern char map[20][32];

// ghost specific
#define MAX_ATTEMPTS 6

// directions
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_NONE
} Direction;

// Ghost structs
typedef struct {
    uint8_t col;
    uint8_t row;
    uint8_t prev_col;
    uint8_t prev_row;
    uint16_t color;
    Direction dir;
    bool in_house;
    bool is_scared;
    long inside_time;
} Ghost;

// global vars:
// Pac-Man struct
typedef struct {
    uint8_t col;
    uint8_t row;
    uint8_t prev_col;
    uint8_t prev_row;
    Direction dir;
    Direction desiredDir;
    uint32_t score;
    uint8_t lives;
} Pacman;

extern Pacman pacman;


// ghosts structs 
extern Ghost blinky;
extern Ghost pinky;
extern Ghost inky;
extern Ghost clyde;

// scared timers
extern int scared_timer;
extern bool ghosts_scared;
#define SCARED_TIME 400 

#endif /* _GAME_LOGIC_H_ */