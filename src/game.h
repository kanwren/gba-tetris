#ifndef GAME_H
#define GAME_H

#include "gba.h"
#include "tetromino.h"

// The number of ticks of the game loop per frame of the game
#define TICKS 4000
#define BASE_DROP_TICKS ((TICKS) * 6)
#define SOFT_DROP_TICKS ((TICKS) / 2)

#define DAS_DELAY ((TICKS))
#define SHIFT_DELAY ((TICKS) * 3)

// The width in pixels of every square in a piece
#define SQUARE_WIDTH 6
#define BOARD_WIDTH_PIXELS ((SQUARE_WIDTH) * (BOARD_WIDTH))
#define BOARD_HEIGHT_PIXELS ((SQUARE_WIDTH) * (BOARD_HEIGHT))

// The coordinates of the top left of the board features
#define BOARD_X (((WIDTH)-(BOARD_WIDTH_PIXELS))/2)
#define BOARD_Y (((HEIGHT)-(BOARD_HEIGHT_PIXELS))/2)
#define NEXT_BOX_X ((BOARD_X)+(BOARD_WIDTH_PIXELS)+18)
#define NEXT_BOX_Y ((BOARD_Y)+(((BOARD_HEIGHT_PIXELS)-(PIECE_WIDTH)*(SQUARE_WIDTH))/2)-24)
#define FACE_X ((BOARD_X)+(BOARD_WIDTH_PIXELS)+18)
#define FACE_Y ((BOARD_Y)+(((BOARD_HEIGHT_PIXELS)-(PIECE_WIDTH)*(SQUARE_WIDTH))/2)+24)

#define NUM_LEVELS 10

typedef struct animation_state {
    int show_prompt;
    int final_screen_shown;
    u16 lose_filled_lines;
} AnimationState;

typedef struct tetris_state {
    // Determine which direction DAS should apply. 0 means there's no sideways
    // movement, +/- 1 means regular movement to the side, and +/- 2 means DAS
    int das_tracking;
    u32 score;
    u32 lines_cleared;
    int tetromino_x;
    int tetromino_y;
    // The next tetromino, visible on the side
    Tetromino queued_tetromino;
    // The current tetromino being used
    Tetromino current_tetromino;
    // Value from 0-3 representing the current rotational state
    int tetromino_orientation;
    // Keeps track of the enum value in order to determine color
    // 0 represents an empty tile
    Tetromino board[BOARD_HEIGHT][BOARD_WIDTH];
} TetrisState;

void reset_game(void);
void start_play(void);

/**
 * Pulls the piece from the queue and attempts to places it on the board.
 * Generates a new piece for the next box. If there's no queued piece, it will
 * generate a random piece and repopulate the queue. If there's already a piece
 * where it tries to place the new piece, set the state anyway and return a
 * nonzero error code.
 */
int spawn_new_piece(TetrisState *state);

/**
 * Attempt to rotate the current falling tetromino. It will rotate clockwise if
 * direction is positive and counterclockwise otherwise. Returns whether or not
 * it succeeded.
 */
int rotate_piece(TetrisState *state, int clockwise);

/**
 * Attempt to move the current tetromino by one square left or right. Returns
 * whether or not it succeeded.
 */
int move_piece(TetrisState *state, int direction);

/**
 * Attempt to drop the current tetromino by one square. Returns whether or not
 * the drop succeeded.
 */
int drop_piece(TetrisState *state);

/**
 * Freezes the current falling tetromino by writing it into the game board.
 */
void freeze_piece(TetrisState *state);

/**
 * Finds the indices of all the lines that need to be cleared, which is at most
 * 4, from highest to lowest. Return these in a supplied array, padded on the
 * end with -1's. Also returns the number of lines to be cleared.
 */
int find_filled_lines(TetrisState *state, int lines_filled[4]);

/**
 * Clears the given lines from the board, shifting everything else down.
 */
void clear_lines(TetrisState *state, int lines_filled[4]);

/**
 * Your current level affects the number of points you earn for clears, so we
 * determine this here.
 */
u32 get_level(u32 lines_cleared);

/**
 * Increase the score for a game based on the number of lines cleared.
 * Note: it's not possible to clear four lines at once without getting a Tetris
 */
void increment_score(TetrisState *state, int lines);

/**
 * Writes black over the squares that a tetromino used to occupy
 */
void clear_piece(Tetromino type, int orientation, int x, int y);

/**
 * Draws the current falling piece on the board.
 */
void draw_piece(TetrisState *state);

void draw_queued_piece(TetrisState *state);

/**
 * Draw any piece at a given screen coordinate
 */
void draw_piece_at(Tetromino type, int orientation, int x, int y);

/**
 * Draws the current state of the board, minus the current falling piece.
 */
void draw_board(TetrisState *state);

/**
 * Draws the score. Does it exactly what it says on the tin.
 */
void draw_score(TetrisState *state);

/**
 * Draw the face to judge your current position.
 */
void draw_face(TetrisState *state);

#endif
