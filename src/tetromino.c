#include "tetromino.h"
#include "gba.h"

Tetromino random_tetromino(void)
{
    return randint(1, NUM_TETROMINOES + 1);
}

unsigned short const tetromino_color[NUM_TETROMINOES + 1] = {
    BLACK,    // empty
    YELLOW,   // O
    GREEN,    // S
    RED,      // Z
    MAGENTA,  // T
    MED_BLUE, // J
    ORANGE,   // L
    CYAN,     // I
};

// Each of these arrays contains four 4x4 grids detailing the filled squares for
// each orientation of each of the pieces. The rotations progress from the
// default orientation clockwise. The I piece is the only piece that extends to
// the leftmost column; like classic NES Tetris, the pieces are right-biased.
unsigned char const tetromino_pattern[NUM_TETROMINOES + 1][4][4][4] = {
    {
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
    },
    // O
    {
        {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        },
    },
    // S
    {
        {
            { 0, 0, 0, 0 },
            { 0, 0, 1, 1 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 1 },
            { 0, 0, 0, 1 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 1, 1 },
            { 0, 1, 1, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 1, 0 },
        },
    },
    // Z
    {
        {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 1, 1 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 1 },
            { 0, 0, 1, 1 },
            { 0, 0, 1, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 1, 1 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 1, 0, 0 },
        },
    },
    // T
    {
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 1, 1, 1 },
            { 0, 0, 1, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 1, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 1, 1, 1 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 1 },
            { 0, 0, 1, 0 },
        },
    },
    // J
    {
        {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 1 },
            { 0, 0, 0, 1 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 1, 1, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 1, 1 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 1, 1 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
        },
    },
    // L
    {
        {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 1 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 1 },
            { 0, 1, 1, 1 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 1 },
        },
    },
    // I
    {
        {
            { 0, 0, 0, 0 },
            { 1, 1, 1, 1 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 1, 0 },
        },
        {
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 1, 1, 1, 1 },
            { 0, 0, 0, 0 },
        },
        {
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
        },
    },
};

int tetromino_fits(Tetromino type, int orientation, int x, int y, Tetromino board[BOARD_HEIGHT][BOARD_WIDTH])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // We don't consider a negative y, since the pieces start above the
            // board and simply aren't drawn
            int out = (x + i >= BOARD_WIDTH) || (x + i < 0) || (y + j >= BOARD_HEIGHT);
            unsigned char block = tetromino_pattern[type][orientation][j][i];
            if (block && (out || board[y + j][x + i])) {
                return 0;
            }
        }
    }
    return 1;
}

