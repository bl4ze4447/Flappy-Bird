//
// Created by Antonie Gabriel Belu on 12.02.2025.
//

#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H
#include <raylib.h>

#define SCREEN_WIDTH        920
#define SCREEN_HEIGHT       520
#define PLAYER_HEIGHT       30
#define PLAYER_WIDTH        40
#define GRAVITY_FORCE       6       // How many pixels does the player drop per frame
#define GRAVITY_ITERATIONS  15
#define MAX_PIPES           6
#define PIPE_WIDTH          100
#define PIPE_SPACE          (PLAYER_HEIGHT * 6)
#define PIPE_MOVEMENT       7      // How fast the pipes move towards the left side of the screen
#define JUMP_FORCE          80      // How many pixels does the player jump
#define JUMP_ITERATIONS     10      // divided in this many steps for a visually smoother jump
                                    // First iteration is doubled, making it
                                    // JUMP_FORCE*2/JUMP_ITERATIONS + (JUMP_FORCE/JUMP_ITERATIONS)

enum GameState {
    RUNNING,
    DIED,
    AWAITING_INPUT
};

struct GameInfo {
    int score;
    enum GameState state;
};

struct Player {
    Rectangle   rect;
    Texture2D   texture_falling;
    Texture2D   texture_jumping;
    Texture2D   texture_slow_falling;

    double      jump_delay;
    double      last_jump_time;
    int         jump_force;
    int         jump_iterations_left;
    bool        can_jump;

    int         small_gravity_iterations;
};

struct Pipe {
    float       x;
    float       x_offset;       // Pipe position when outside the screen
    float       width;

    float       height_up;
    float       height_down;
    float       y_up;
    float       y_down;

    bool        score_consumed; // Did the player get +1 to the score from this pipe?
};

struct Pipes {
    struct Pipe array[MAX_PIPES];
    int         count;
    int         last_pipe_offset;
};

#endif //ENGINE_TYPES_H
