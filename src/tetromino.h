#ifndef TETROMINO_H
#define TETROMINO_H

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define NUM_TETROMINOES 7
#define PIECE_WIDTH 4

/**
 * Type of a tetromino. Starts at 1, so 0 can be used to represent an absence of
 * a tetromino.
 */
typedef enum tetromino {
    O = 1, S, Z, T, J, L, I
} Tetromino;

/**
 * Generate a random nonempty tetromino
 */
Tetromino random_tetromino(void);

/**
 * Array containing the color of the squares in the tetromino, one for each
 * tetromino value
 */
extern unsigned short const tetromino_color[NUM_TETROMINOES + 1];

/**
 * Each entry corresponds to a shape; the entry is an array of four patterns of
 * the piece, one for each orientation. The size of the array is determined by
 * the size of the corresponding tetromino, so each element is a pointer to an
 * array of patterns. This structure is annoying, but you can get a pattern like
 * so:
 *
 * unsgined char const * pattern = &tetromino_pattern[type][size * size * orientation];
 *
 */
extern unsigned char const tetromino_pattern[NUM_TETROMINOES + 1][4][4][4];

/**
 * Check if a tetromino of a given type fits in the board at a given space.
 * Returns 0 if it cannot fit, or 1 otherwise.
 */
int tetromino_fits(Tetromino type, int orientation, int x, int y, Tetromino board[BOARD_HEIGHT][BOARD_WIDTH]);

#endif
