#include "decl.h"
#include "stm32f0xx.h"
#include <stdlib.h>

extern volatile bool ghost_tick;

void draw_ghost(Ghost *g) {
    uint16_t x = g->col * TILE_SIZE;
    uint16_t y = g->row * TILE_SIZE;

    lcd_set_window(x, y, x + 11, y + 11);

    for (int row = 0; row < 12; row++) {
        for (int col = 0; col < 12; col++) {
            uint16_t px = COLOR_BLACK;

            int dx = col - 6;
            if (row < 6 && dx * dx + (row - 5) * (row - 5) <= 25) {
                px = g->is_scared ? COLOR_BLUE : g->color;
            }
            if (row >= 6) {
                px = g->is_scared ? COLOR_BLUE : g->color;
            }

            if ((row >= 3 && row <= 5) &&
                ((col >= 3 && col <= 4) || (col >= 7 && col <= 8))) {
                px = COLOR_WHITE;
            }
            if ((row == 4) && (col == 4 || col == 8)) {
                px = COLOR_BLACK;
            }

            lcd_send_data16(px);
        }
    }
}



void move_ghost_up(Ghost *g) {
    erase_tile(g->col, g->row);
    if (g->row > 0) g->row--;
    draw_ghost(g);
}

void move_ghost_down(Ghost *g) {
    erase_tile(g->col, g->row);
    if (g->row < 19) g->row++;
    draw_ghost(g);
}

void move_ghost_left(Ghost *g) {
    erase_tile(g->col, g->row);
    if (g->col > 0) g->col--;
    draw_ghost(g);
}

void move_ghost_right(Ghost *g) {
    erase_tile(g->col, g->row);
    if (g->col < 29) g->col++;
    draw_ghost(g);
}

Direction random_direction() {
    return rand() % 4; // returns 0–3
}

bool is_intersection(uint8_t col, uint8_t row) {
    uint8_t paths = 0;

    if (!is_wall(col, row - 1)) paths++;  // up
    if (!is_wall(col, row + 1)) paths++;  // down
    if (!is_wall(col - 1, row)) paths++;  // left
    if (!is_wall(col + 1, row)) paths++;  // right

    return paths >= 3;  // An intersection = 3 or more open paths
}

void release_ghost(Ghost *g) {
    g->dir = DIR_UP;
}

void reset_ghost(Ghost *g) {
    erase_tile(g->col, g->row);

    if (g->color == COLOR_BLINKY) {
        g->col = BLINKY_ORIG_COL;
        g->row = BLINKY_ORIG_ROW;
    } else if (g->color == COLOR_PINKY) {
        g->col = PINKY_ORIG_COL;
        g->row = PINKY_ORIG_ROW;
    } else if (g->color == COLOR_INKY) {
        g->col = INKY_ORIG_COL;
        g->row = INKY_ORIG_ROW;
    } else if (g->color == COLOR_CLYDE) {
        g->col = CLYDE_ORIG_COL;
        g->row = CLYDE_ORIG_ROW;
    }

    g->dir = DIR_NONE;
    g->in_house = true;
    g->is_scared = false;
    g->inside_time = 0;

    draw_ghost(g);
}


void move_ghost(Ghost *g) {
    g->prev_row = g->row;
    g->prev_col = g->col;
    uint8_t attempts = 0;
    Direction dir = g->dir;
    // this checks if ghost is at the teleportation point
    if (g->col == 0 && g->row == 10) {
        erase_tile(g->col, g->row);
        g->col = 29;  // teleport to the other side
        g->dir = DIR_LEFT;  // set direction to right
    } else if (g->col == 29 && g->row == 10) {
        erase_tile(g->col, g->row);
        g->col = 0;  // teleport to the other side
        g->dir = DIR_RIGHT;  // set direction to left
    }
    if (is_intersection(g->col, g->row)) {
        dir = random_direction();  // choose randomly
    }
    while (attempts < MAX_ATTEMPTS) {
        switch (dir) {
            case DIR_UP:
                if (!is_wall(g->col, g->row - 1)) {
                    move_ghost_up(g);
                    g->dir = dir;
                    return;
                }
                break;
            case DIR_DOWN:
                if (!is_wall(g->col, g->row + 1)) {
                    // this if prevents it from going into the house
                    if ((g->row == 8) && (g->col == 14 || g->col == 15 )) break;
                    move_ghost_down(g);
                    g->dir = dir;
                    return;
                }
                break;
            case DIR_LEFT:
                if (!is_wall(g->col - 1, g->row)) {
                    move_ghost_left(g);
                    g->dir = dir;
                    return;
                }
                break;
            case DIR_RIGHT:
                if (!is_wall(g->col + 1, g->row)) {
                    move_ghost_right(g);
                    g->dir = dir;
                    return;
                }
                break;
        }
        attempts++;
        dir = random_direction();
    }
}
