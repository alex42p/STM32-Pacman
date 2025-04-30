#include "decl.h"
#include "stm32f0xx.h"
#include <stdio.h>

extern volatile bool pacman_tick;

// Draw Pac-Man as a 12×12 circle at tile (p->col, p->row)
void draw_pacman(Pacman *p) {
    uint16_t x0 = p->col * TILE_SIZE;
    uint16_t y0 = p->row * TILE_SIZE;

    lcd_set_window(x0, y0, x0 + 11, y0 + 11);
    for (int r = 0; r < 12; r++) {
        for (int c = 0; c < 12; c++) {
            int dx = c - 6, dy = r - 6;
            if (dx*dx + dy*dy <= 6*6) {
                lcd_send_data16(COLOR_PACMAN);
            } else {
                lcd_send_data16(COLOR_BLACK);
            }
        }
    }
}


void check_ghost_collision(Pacman *p, Ghost *g1, Ghost *g2, Ghost *g3, Ghost *g4) {
    Ghost *ghosts[4] = {g1, g2, g3, g4};

    for (int i = 0; i < 4; i++) {
        Ghost *g = ghosts[i];

        bool same_tile = (p->row == g->row && p->col == g->col);
        bool cross_path = (p->row == g->prev_row && p->col == g->prev_col &&
                           p->prev_row == g->row && p->prev_col == g->col);

        if (same_tile || cross_path) {
            if (g->is_scared) {
                // Pac-Man eats the ghost
                erase_tile(g->col, g->row);
                reset_ghost(g);
                p->score += 200;  // bonus points for eating ghost
                nano_wait(500000000); 
                draw_ghost(g);
            } else {
                // Pac-Man dies
                if (p->lives > 0) {
                    p->lives--;
                }

                erase_tile(p->col, p->row);
                p->col = PACMAN_ORIG_COL;
                p->row = PACMAN_ORIG_ROW;
                p->dir = DIR_NONE;
                p->desiredDir = DIR_NONE;
                draw_pacman(p);
                nano_wait(1000000000); 
                reset_ghost(g1);
                reset_ghost(g2);
                reset_ghost(g3);
                reset_ghost(g4);
            }
        }
    }
}

void move_pacman(Pacman *p) {
    p->prev_row = p->row;
    p->prev_col = p->col;
    // 1) Tunnel wrap
    if (p->col == 0 && p->row == 10 && p->dir == DIR_LEFT) {
        erase_tile(p->col, p->row);
        p->col = 29;
        p->dir = DIR_LEFT;
        draw_pacman(p);
        return;
    } else if (p->col == 29 && p->row == 10 && p->dir == DIR_RIGHT) {
        erase_tile(p->col, p->row);
        p->col = 0;
        p->dir = DIR_RIGHT;
        draw_pacman(p);
        return;
    }

    // 2) First, check if desired_dir is possible
    uint8_t ndc = p->col, ndr = p->row;
    switch (p->desiredDir) {
        case DIR_UP:    if (ndr > 0)    ndr--; break;
        case DIR_DOWN:  if (ndr < 19)   ndr++; break;
        case DIR_LEFT:  if (ndc > 0)    ndc--; break;
        case DIR_RIGHT: if (ndc < 29)   ndc++; break;
        default: break;
    }
    if (!is_wall(ndc, ndr)) {
        // It's possible to go in desired direction
        p->dir = p->desiredDir;
    }

    // 3) Move in current direction (even if it's not the desired one)
    uint8_t nc = p->col, nr = p->row;
    switch (p->dir) {
        case DIR_UP:    if (nr > 0)    nr--; break;
        case DIR_DOWN:  if (nr < 19)   nr++; break;
        case DIR_LEFT:  if (nc > 0)    nc--; break;
        case DIR_RIGHT: if (nc < 29)   nc++; break;
        default: return;
    }

    // 4) Collision check
    if (!is_wall(nc, nr)) {
        erase_tile(p->col, p->row);
        p->col = nc;
        p->row = nr;

        if (map[p->row][p->col] == '.') {
            // char buffer[128];
            // snprintf(buffer, sizeof(buffer), "Score: %d\r\n", p->score);
            // uart_send_string(buffer);
            map[p->row][p->col] = ' ';   // eat the pellet
            p->score += 10;              // increase score
            // init_tim6();
            play_audio();
        }
        if (map[p->row][p->col] == 'o') {
            map[p->row][p->col] = ' ';   // eat the power pellet
            p->score += 50;              // increase score
            // SCARE DEM GHOSTS 
            blinky.is_scared = true;
            pinky.is_scared = true;
            inky.is_scared = true;
            clyde.is_scared = true;
            ghosts_scared = true;
            scared_timer = SCARED_TIME;
        }
        draw_pacman(p);
    }
}
