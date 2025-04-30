#include "decl.h"

void load_background(void) {
    pacman.row = PACMAN_ORIG_ROW;
    pacman.col = PACMAN_ORIG_COL;
    pacman.prev_row = PACMAN_ORIG_ROW;
    pacman.prev_col = PACMAN_ORIG_COL;
    blinky.row = BLINKY_ORIG_ROW;
    blinky.col = BLINKY_ORIG_COL;
    blinky.prev_row = BLINKY_ORIG_ROW;
    blinky.prev_col = BLINKY_ORIG_COL;
    inky.row = INKY_ORIG_ROW;
    inky.col = INKY_ORIG_COL;
    inky.prev_row = INKY_ORIG_ROW;
    inky.prev_col = INKY_ORIG_COL;
    pinky.row = PINKY_ORIG_ROW;
    pinky.col = PINKY_ORIG_COL;
    pinky.prev_row = PINKY_ORIG_ROW;
    pinky.prev_col = PINKY_ORIG_COL;
    clyde.row = CLYDE_ORIG_ROW;
    clyde.col = CLYDE_ORIG_COL;
    clyde.prev_row = CLYDE_ORIG_ROW;
    clyde.prev_col = CLYDE_ORIG_COL;
    blinky.is_scared = false;
    blinky.in_house = true;
    inky.is_scared = false;
    inky.in_house = true;
    pinky.is_scared = false;
    pinky.in_house = true;
    clyde.is_scared = false;
    clyde.in_house = true;
    
    
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 30; col++) {
            uint16_t color = (map[row][col] == '#') ? 0x001F : 0x0000;

            // Set window for this 16x16 tile
            lcd_send_command(0x2A); // Column addr
            lcd_send_data((col * 16) >> 8);
            lcd_send_data((col * 16) & 0xFF);
            lcd_send_data((col * 16 + 15) >> 8);
            lcd_send_data((col * 16 + 15) & 0xFF);

            lcd_send_command(0x2B); // Row addr
            lcd_send_data((row * 16) >> 8);
            lcd_send_data((row * 16) & 0xFF);
            lcd_send_data((row * 16 + 15) >> 8);
            lcd_send_data((row * 16 + 15) & 0xFF);

            lcd_send_command(0x2C); // Write RAM

            // Send 16x16 = 256 pixels
            for (int i = 0; i < 256; i++) {
                lcd_send_data(color >> 8);
                lcd_send_data(color & 0xFF);
            }
            if (map[row][col] == '.') {
                draw_pellet(col, row);
            }
            if (map[row][col] == 'o') {
                draw_power_pellet(col, row);
            }
        }
    }
    // draw sprites in initial pos
    draw_pacman(&pacman);
    draw_ghost(&blinky);   
    draw_ghost(&pinky);     
    draw_ghost(&inky);
    draw_ghost(&clyde);      
}

void erase_tile(uint8_t col, uint8_t row) {
    uint16_t color = (map[row][col] == '#') ? COLOR_BLUE : COLOR_BLACK;

    lcd_set_window(col * 16, row * 16, col * 16 + 15, row * 16 + 15);
    for (int i = 0; i < 256; i++) {
        lcd_send_data16(color);
    }
    if (map[row][col] == '.') {
        draw_pellet(col, row);
    }
    if (map[row][col] == 'o') {
        draw_power_pellet(col, row);
    }
}

// collision detection function
// returns true if the tile is a wall, false otherwise
bool is_wall(uint8_t col, uint8_t row) {
    extern char map[20][32];
    if (col >= 32 || row >= 20) return true; // out of bounds = treat as wall
    return map[row][col] == '#' || map[row][col] == '!';
}

void draw_pellet(uint8_t col, uint8_t row) {
    uint16_t cx = col * TILE_SIZE + TILE_SIZE / 2;
    uint16_t cy = row * TILE_SIZE + TILE_SIZE / 2;

    lcd_set_window(cx - 1, cy - 1, cx + 1, cy + 1); // 3x3 white dot

    for (int i = 0; i < 9; i++) {
        lcd_send_data16(COLOR_WHITE);
    }
}
void draw_power_pellet(uint8_t col, uint8_t row) {
    uint16_t cx = col * TILE_SIZE + TILE_SIZE / 2;
    uint16_t cy = row * TILE_SIZE + TILE_SIZE / 2;

    lcd_set_window(cx - 3, cy - 3, cx + 3, cy + 3); // 7x7 white dot

    for (int i = 0; i < 49; i++) {
        lcd_send_data16(COLOR_WHITE);
    }
}

const char gameover_map[20][32] = {
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "  #####    ##    #    #  #####  ",
    " #        #  #   ##  ##  #      ",
    " #  ###  #    #  # ## #  #####  ",
    " #    #  ######  #    #  #      ",
    "  #####  #    #  #    #  #####  ",
    "                                ",
    "  ####   #    #  #####  #####   ",
    " #    #  #    #  #      #    #  ",
    " #    #  #    #  #####  #####   ",
    " #    #   #  #   #      #   #   ",
    "  ####     ##    #####  #    #  ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                "
};


void load_gameover(void) {
    lcd_fill_screen(COLOR_BLACK); // Clear screen
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 30; col++) {
            uint16_t color = (gameover_map[row][col] == '#') ? COLOR_RED : 0x0000;

            // Set window for this 16x16 tile
            lcd_send_command(0x2A); // Column addr
            lcd_send_data((col * 16) >> 8);
            lcd_send_data((col * 16) & 0xFF);
            lcd_send_data((col * 16 + 15) >> 8);
            lcd_send_data((col * 16 + 15) & 0xFF);

            lcd_send_command(0x2B); // Row addr
            lcd_send_data((row * 16) >> 8);
            lcd_send_data((row * 16) & 0xFF);
            lcd_send_data((row * 16 + 15) >> 8);
            lcd_send_data((row * 16 + 15) & 0xFF);

            lcd_send_command(0x2C); // Write RAM

            // Send 16x16 = 256 pixels
            for (int i = 0; i < 256; i++) {
                lcd_send_data(color >> 8);
                lcd_send_data(color & 0xFF);
            }
        }
    }
}

bool check_win(void) {
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 32; col++) {
            if (map[row][col] == '.' || map[row][col] == 'o') {
                // Still a pellet or power pellet left
                return false;
            }
        }
    }
    return true; // No pellets left — player wins
}

const char gamewon_map[20][32] = {
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    " #    #   ###   #    #          ",
    "  #  #   #   #  #    #          ",
    "   ##    #   #  #    #          ",
    "   ##    #   #  #    #          ",
    "   ##     ###    ####           ",
    "                                ",
    " #     #   ###   #    #         ",
    " #  #  #  #   #  ##   #         ",
    " #  #  #  #   #  # #  #         ",
    " #  #  #  #   #  #  # #         ",
    "  ## ##    ###   #   ##         ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                "
};

void load_gamewon(void) {
    lcd_fill_screen(COLOR_BLACK); // Clear screen
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 30; col++) {
            uint16_t color = (gamewon_map[row][col] == '#') ? 0x001F : 0x0000;

            // Set window for this 16x16 tile
            lcd_send_command(0x2A); // Column addr
            lcd_send_data((col * 16) >> 8);
            lcd_send_data((col * 16) & 0xFF);
            lcd_send_data((col * 16 + 15) >> 8);
            lcd_send_data((col * 16 + 15) & 0xFF);

            lcd_send_command(0x2B); // Row addr
            lcd_send_data((row * 16) >> 8);
            lcd_send_data((row * 16) & 0xFF);
            lcd_send_data((row * 16 + 15) >> 8);
            lcd_send_data((row * 16 + 15) & 0xFF);

            lcd_send_command(0x2C); // Write RAM

            // Send 16x16 = 256 pixels
            for (int i = 0; i < 256; i++) {
                lcd_send_data(color >> 8);
                lcd_send_data(color & 0xFF);
            }
        }
    }
}