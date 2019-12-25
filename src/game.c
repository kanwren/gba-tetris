#include <stdio.h>
#include <stdlib.h>
#include "gba.h"
#include "game.h"

#include "images/garbage.h"
#include "images/start_screen.h"
#include "images/play_screen.h"
#include "images/face_happy.h"
#include "images/face_neutral.h"
#include "images/face_sad.h"
#include "images/face_sunglasses.h"

typedef enum {
    START,
    PLAY,
    WIN,
    LOSE,
} GBAState;

static const AnimationState default_animation_state = {
    .show_prompt = 1,
    .final_screen_shown = 0,
    .lose_filled_lines = 0,
};

static const TetrisState default_game_state = {
    .das_tracking = 0,
    .score = 0,
    .lines_cleared = 0,
    .tetromino_x = 0,
    .tetromino_y = 0,
    .queued_tetromino = 0,
    .current_tetromino = 0,
    .tetromino_orientation = 0,
    .board = {{ 0 }},
};

AnimationState animation_state = { 0 };

TetrisState game_state = { 0 };

int main(void) {
    REG_DISPCNT = MODE3 | BG2_ENABLE;

    // Save current and previous state of button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

    // Load initial game state
    GBAState state = START;

    char press_start[] = "press START";
    int press_start_x = (WIDTH - (6 * sizeof(press_start) / sizeof(*press_start))) / 2;
    reset_game();

    u32 count = 0;
    u32 das_count = 0;
    while (1) {
        currentButtons = BUTTONS;

        switch (state) {
            case START:
                if (count >= (TICKS * 25)) {
                    int show_prompt = animation_state.show_prompt;
                    if (show_prompt) {
                        waitForVBlank();
                        drawString(press_start_x, 50, press_start, WHITE);
                    } else {
                        waitForVBlank();
                        drawString(press_start_x, 50, press_start, BLACK);
                    }
                    animation_state.show_prompt = !show_prompt;
                    count = 0;
                }
                if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
                    start_play();
                    seed_qran(count);
                    state = PLAY;
                } else if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
                    waitForVBlank();
                    drawFullScreenImageDMA(start_screen);
                    state = START;
                    count = 0;
                }

                break;

            /**
             * On every tick:
             * - If there is no current, spawn a new piece.
             * - If there is a current piece:
             *    - Try to drop the piece
             *    - If it fails:
             *       - Write the piece into the board
             *       - Clear any filled lines
             *       - Increment the score
             */
            case PLAY:
                if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
                    reset_game();
                    state = START;
                    count = 0;
                } else if (game_state.current_tetromino) {
                    Tetromino cur = game_state.current_tetromino;
                    int old_x = game_state.tetromino_x;
                    int old_y = game_state.tetromino_y;
                    int old_orientation = game_state.tetromino_orientation;
                    // If LEFT or RIGHT are released, cancel DAS in those
                    // directions
                    if (KEY_JUST_RELEASED(BUTTON_LEFT, currentButtons, previousButtons) && game_state.das_tracking < 0) {
                        game_state.das_tracking = 0;
                    }
                    if (KEY_JUST_RELEASED(BUTTON_RIGHT, currentButtons, previousButtons) && game_state.das_tracking > 0) {
                        game_state.das_tracking = 0;
                    }
                    int das = game_state.das_tracking;

                    if (KEY_JUST_PRESSED(BUTTON_A, currentButtons, previousButtons)) {
                        // Rotate clockwise
                        int succeeded = rotate_piece(&game_state, 1);
                        if (succeeded) {
                            waitForVBlank();
                            clear_piece(cur, old_orientation, old_x, old_y);
                            draw_piece(&game_state);
                        }
                    } else if (KEY_JUST_PRESSED(BUTTON_B, currentButtons, previousButtons)) {
                        // Rotate counterclockwise
                        int succeeded = rotate_piece(&game_state, -1);
                        if (succeeded) {
                            waitForVBlank();
                            clear_piece(cur, old_orientation, old_x, old_y);
                            draw_piece(&game_state);
                        }
                    } else if (KEY_JUST_PRESSED(BUTTON_LEFT, currentButtons, previousButtons)
                            || (KEY_DOWN(BUTTON_LEFT, currentButtons)
                                && ((das < -1 && das_count >= DAS_DELAY)
                                    || (das <= 0 && das_count >= SHIFT_DELAY)))) {
                        // Move left
                        int succeeded = move_piece(&game_state, -1);
                        if (succeeded) {
                            waitForVBlank();
                            clear_piece(cur, old_orientation, old_x, old_y);
                            draw_piece(&game_state);
                        }
                        game_state.das_tracking--;
                        das_count = 0;
                    } else if (KEY_JUST_PRESSED(BUTTON_RIGHT, currentButtons, previousButtons)
                            || (KEY_DOWN(BUTTON_RIGHT, currentButtons)
                                && ((das > 1 && das_count >= DAS_DELAY)
                                    || (das >= 0 && das_count >= SHIFT_DELAY)))) {
                        // Move right
                        int succeeded = move_piece(&game_state, 1);
                        if (succeeded) {
                            waitForVBlank();
                            clear_piece(cur, old_orientation, old_x, old_y);
                            draw_piece(&game_state);
                        }
                        game_state.das_tracking++;
                        das_count = 0;
                    } else {
                        u32 level = get_level(game_state.lines_cleared);
                        u32 multiplier = (level / 2) + 1;
                        u32 freq = KEY_DOWN(BUTTON_DOWN, currentButtons)
                            ? SOFT_DROP_TICKS
                            : BASE_DROP_TICKS / multiplier;
                        if (count >= freq) {
                            int could_drop = drop_piece(&game_state);
                            if (!could_drop) {
                                freeze_piece(&game_state);
                                draw_face(&game_state);
                            } else {
                                waitForVBlank();
                                clear_piece(cur, old_orientation, old_x, old_y);
                                draw_piece(&game_state);
                            }
                            count = 0;
                        }
                    }
                } else {
                    // No current tetromino
                    int filled[4];
                    int clear = find_filled_lines(&game_state, filled);
                    if (clear) {
                        game_state.lines_cleared += clear;
                        increment_score(&game_state, clear);
                        clear_lines(&game_state, filled);
                        waitForVBlank();
                        draw_board(&game_state);
                        draw_score(&game_state);
                    }
                    if (get_level(game_state.lines_cleared) >= NUM_LEVELS) {
                        count = 0;
                        state = WIN;
                        break;
                    }
                    int failed = spawn_new_piece(&game_state);
                    if (failed) {
                        count = 0;
                        state = LOSE;
                    }
                    // Like in Classic Tetris, still show the piece that spawned
                    // even after you've lost
                    waitForVBlank();
                    draw_piece(&game_state);
                    draw_queued_piece(&game_state);
                }

                break;

            case WIN:
                if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
                    reset_game();
                    state = START;
                    count = 0;
                } else if (!animation_state.final_screen_shown) {
                    waitForVBlank();
                    fillScreenDMA(BLACK);
                    draw_piece_at(T, 0, (WIDTH - 5 * SQUARE_WIDTH) / 2, (HEIGHT - 5 * SQUARE_WIDTH) / 2 - 10);

                    drawCenteredString(0, -35, WIDTH, HEIGHT, "YOU WIN!", GREEN);
                    char buf[24];
                    snprintf(buf, 24, "Your score was: %d", game_state.score);
                    drawCenteredString(0, 20, WIDTH, HEIGHT, buf, WHITE);
                    snprintf(buf, 24, "Lines cleared: %d", game_state.lines_cleared);
                    drawCenteredString(0, 30, WIDTH, HEIGHT, buf, WHITE);
                    animation_state.final_screen_shown = 1;
                }

                break;

            case LOSE:
                if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
                    reset_game();
                    state = START;
                    count = 0;
                } else if (animation_state.lose_filled_lines < BOARD_HEIGHT) {
                    if (count >= TICKS) {
                        animation_state.lose_filled_lines++;
                        count = 0;
                    }
                } else if (!animation_state.final_screen_shown) {
                    waitForVBlank();
                    fillScreenDMA(BLACK);
                    draw_piece_at(Z, 0, (WIDTH - 5 * SQUARE_WIDTH) / 2, (HEIGHT - 5 * SQUARE_WIDTH) / 2 - 5);

                    drawCenteredString(0, -35, WIDTH, HEIGHT, "YOU LOSE", RED);
                    char buf[24];
                    snprintf(buf, 24, "Your score was: %d", game_state.score);
                    drawCenteredString(0, 20, WIDTH, HEIGHT, buf, WHITE);
                    snprintf(buf, 24, "Lines cleared: %d", game_state.lines_cleared);
                    drawCenteredString(0, 30, WIDTH, HEIGHT, buf, WHITE);
                    animation_state.final_screen_shown = 1;
                }
                break;
        }

        previousButtons = currentButtons;
        count++;
        das_count++;
    }

    return 0;
}

void reset_game(void)
{
    animation_state = default_animation_state;

    waitForVBlank();
    drawFullScreenImageDMA(start_screen);
}

void start_play(void)
{
    game_state = default_game_state;
    waitForVBlank();
    drawFullScreenImageDMA(play_screen);
    draw_score(&game_state);
    draw_board(&game_state);
    draw_face(&game_state);
}

__attribute__((always_inline))
static inline u16 darken_color(u16 color)
{
    // 0b0111101111011110
    return (color & 0x7bde) >> 1;
}

__attribute__((always_inline))
static inline u16 lighten_color(u16 color)
{
    // 0b0011110111101111
    return (color & 0x3def) << 1;
}

int spawn_new_piece(TetrisState *state)
{
    Tetromino queued_piece = random_tetromino();
    Tetromino new_piece;
    if (state->queued_tetromino) {
        new_piece = state->queued_tetromino;
    } else {
        new_piece = random_tetromino();
    }
    // Center the piece
    int x = BOARD_WIDTH / 2 - (PIECE_WIDTH / 2);
    // The piece actually starts at -1, but the piece is first shown when it's
    // dropped for the first time
    int y = -1;
    int orientation = 0;
    // Update state
    state->queued_tetromino = queued_piece;
    state->current_tetromino = new_piece;
    state->tetromino_x = x;
    state->tetromino_y = y;
    state->tetromino_orientation = orientation;
    // If piece does not fit, return error code to signal game over
    return !tetromino_fits(new_piece, orientation, x, y, state->board);
}

int rotate_piece(TetrisState *state, int clockwise)
{
    int new_orientation = state->tetromino_orientation;
    if (clockwise > 0) {
        new_orientation++;
    } else {
        new_orientation--;
    }
    // C's modulus is not a modulus, so we make sure it's not negative by adding
    // 4 first
    new_orientation += 4;
    new_orientation %= 4;

    Tetromino type = state->current_tetromino;
    int x = state->tetromino_x;
    int y = state->tetromino_y;
    if (tetromino_fits(type, new_orientation, x, y, state->board)) {
        state->tetromino_orientation = new_orientation;
        return 1;
    }
    return 0;
}

int move_piece(TetrisState *state, int direction)
{
    int new_x = state->tetromino_x;
    if (direction > 0) {
        new_x++;
    } else {
        new_x--;
    }

    int y = state->tetromino_y;
    int orientation = state->tetromino_orientation;
    Tetromino type = state->current_tetromino;
    if (tetromino_fits(type, orientation, new_x, y, state->board)) {
        state->tetromino_x = new_x;
        return 1;
    }
    return 0;
}

int drop_piece(TetrisState *state)
{
    int new_y = state->tetromino_y + 1;

    int x = state->tetromino_x;
    int orientation = state->tetromino_orientation;
    Tetromino type = state->current_tetromino;
    if (tetromino_fits(type, orientation, x, new_y, state->board)) {
        state->tetromino_y = new_y;
        return 1;
    } else {
        return 0;
    }
}

void freeze_piece(TetrisState *state)
{
    Tetromino type = state->current_tetromino;
    int x = state->tetromino_x;
    int y = state->tetromino_y;
    int orientation = state->tetromino_orientation;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int square = tetromino_pattern[type][orientation][j][i];
            if (square) {
                game_state.board[y + j][x + i] = type;
            }
        }
    }
    state->current_tetromino = 0;
}

int find_filled_lines(TetrisState *state, int lines_filled[4])
{
    int count = 0;
    int j = 0;
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        int filled = 1;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!state->board[y][x]) {
                filled = 0;
                break;
            }
        }
        if (filled) {
            count++;
            lines_filled[j++] = y;
        }
    }
    for (int i = j; i < 4; i++) {
        lines_filled[i] = -1;
    }
    return count;
}

void clear_lines(TetrisState *state, int lines_filled[4])
{
    // Move down pieces
    int d = BOARD_HEIGHT - 1;
    int j = 0;
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        // If marked for removal, clear the row and don't advance the
        // destination
        if (lines_filled[j] == y) {
            j++;
            for (int x = 0; x < BOARD_WIDTH; x++) {
                state->board[d][x] = 0;
            }
        } else {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                state->board[d][x] = state->board[y][x];
            }
            d--;
        }
    }
    // Clear the leftover rows
    for (int y = d; d >= 0; d--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            state->board[y][x] = 0;
        }
    }

    if (lines_filled[3] > 0) {
        drawImageDMA(FACE_X, FACE_Y, FACE_SUNGLASSES_WIDTH, FACE_SUNGLASSES_HEIGHT, face_sunglasses);
    } else {
        draw_face(state);
    }
}

inline u32 get_level(u32 lines_cleared)
{
    return lines_cleared / 4;
}

void increment_score(TetrisState *state, int lines)
{
    u32 cleared = state->lines_cleared;
    u32 level = get_level(cleared);
    u32 multiplier;
    if (lines == 1) {
        multiplier = 40;
    } else if (lines == 2) {
        multiplier = 100;
    } else if (lines == 3) {
        multiplier = 100;
    } else if (lines == 4) {
        multiplier = 1200;
    } else {
        return;
    }
    state->score += multiplier * (level + 1);
}

void clear_piece(Tetromino type, int orientation, int x, int y)
{
    int actual_x = BOARD_X + (SQUARE_WIDTH * x);
    int actual_y = BOARD_Y + (SQUARE_WIDTH * y);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int has_square = tetromino_pattern[type][orientation][j][i];
            int target_x = actual_x + i * SQUARE_WIDTH;
            int target_y = actual_y + j * SQUARE_WIDTH;
            if (y + j >= 0 && has_square) {
                drawRectDMA(target_x, target_y, SQUARE_WIDTH, SQUARE_WIDTH, BLACK);
            }
        }
    }
}

void draw_piece(TetrisState *state)
{
    int piece_x = state->tetromino_x;
    int piece_y = state->tetromino_y;
    int actual_x = BOARD_X + (SQUARE_WIDTH * piece_x);
    int actual_y = BOARD_Y + (SQUARE_WIDTH * piece_y);
    Tetromino type = state->current_tetromino;
    u16 color = tetromino_color[type];
    u16 darker = darken_color(color);
    int orientation = state->tetromino_orientation;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int has_square = tetromino_pattern[type][orientation][j][i];
            int target_x = actual_x + i * SQUARE_WIDTH;
            int target_y = actual_y + j * SQUARE_WIDTH;
            // It's possible to ask for a piece to be drawn at a negative y
            if (piece_y + j >= 0 && has_square) {
                drawRectDMA(target_x, target_y, SQUARE_WIDTH, SQUARE_WIDTH, darker);
                drawRectDMA(target_x + 1, target_y + 1, SQUARE_WIDTH - 2, SQUARE_WIDTH - 2, color);
            }
        }
    }
}

void draw_queued_piece(TetrisState *state)
{
    Tetromino type = state->queued_tetromino;
    if (!type) {
        return;
    }
    // Clear the next box
    drawRectDMA(NEXT_BOX_X, NEXT_BOX_Y, PIECE_WIDTH * SQUARE_WIDTH, PIECE_WIDTH * SQUARE_WIDTH, BLACK);
    u16 color = tetromino_color[type];
    u16 darker = darken_color(color);
    int orientation = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int has_square = tetromino_pattern[type][orientation][j][i];
            if (has_square) {
                int target_x = NEXT_BOX_X + i * SQUARE_WIDTH;
                int target_y = NEXT_BOX_Y + j * SQUARE_WIDTH;
                drawRectDMA(target_x, target_y, SQUARE_WIDTH, SQUARE_WIDTH, darker);
                drawRectDMA(target_x + 1, target_y + 1, SQUARE_WIDTH - 2, SQUARE_WIDTH - 2, color);
            }
        }
    }
}

void draw_piece_at(Tetromino type, int orientation, int x, int y)
{
    if (!type) {
        return;
    }
    u16 color = tetromino_color[type];
    u16 darker = darken_color(color);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int has_square = tetromino_pattern[type][orientation][j][i];
            if (has_square) {
                int target_x = x + i * SQUARE_WIDTH;
                int target_y = y + j * SQUARE_WIDTH;
                drawRectDMA(target_x, target_y, SQUARE_WIDTH, SQUARE_WIDTH, darker);
                drawRectDMA(target_x + 1, target_y + 1, SQUARE_WIDTH - 2, SQUARE_WIDTH - 2, color);
            }
        }
    }
}

void draw_board(TetrisState *state)
{
    drawRectDMA(BOARD_X, BOARD_Y, BOARD_WIDTH, BOARD_HEIGHT, BLACK);
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            int actual_x = BOARD_X + (SQUARE_WIDTH * x);
            int actual_y = BOARD_Y + (SQUARE_WIDTH * y);
            Tetromino type = state->board[y][x];
            u16 color = tetromino_color[type];
            u16 darker = darken_color(color);
            drawRectDMA(actual_x, actual_y, SQUARE_WIDTH, SQUARE_WIDTH, darker);
            drawRectDMA(actual_x + 1, actual_y + 1, SQUARE_WIDTH - 2, SQUARE_WIDTH - 2, color);
        }
    }
}

void draw_score(TetrisState *state)
{
    char buf[19];
    u32 lines = state->lines_cleared;
    u32 level = get_level(lines);
    u32 score = state->score;

    snprintf(buf, 19, "LEVEL: %d", level);
    overlayString(7, 35, buf, WHITE, BLACK);

    snprintf(buf, 19, "LINES: %d", lines);
    overlayString(7, 45, buf, WHITE, BLACK);

    snprintf(buf, 19, "SCORE: %d", score);
    overlayString(7, 55, buf, WHITE, BLACK);
}

void draw_face(TetrisState *state)
{
    int height = 20;
    for (int y = 0; y < BOARD_HEIGHT && height >= 20; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (state->board[y][x]) {
                height = y;
                break;
            }
        }
    }
    if (height >= BOARD_HEIGHT * 4 / 7) {
        drawImageDMA(FACE_X, FACE_Y, FACE_HAPPY_WIDTH, FACE_HAPPY_HEIGHT, face_happy);
    } else if (height >= BOARD_HEIGHT / 3) {
        drawImageDMA(FACE_X, FACE_Y, FACE_NEUTRAL_WIDTH, FACE_NEUTRAL_HEIGHT, face_neutral);
    } else {
        drawImageDMA(FACE_X, FACE_Y, FACE_SAD_WIDTH, FACE_SAD_HEIGHT, face_sad);
    }
}

