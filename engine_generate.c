//
// Created by Antonie Gabriel Belu on 12.02.2025.
//

#include "engine_generate.h"
#include <stddef.h>

void init_pipe(struct Pipe * pipe, const int offset) {
    if (pipe == NULL)
        return;

    pipe->x = SCREEN_WIDTH + offset;
    pipe->width = PIPE_WIDTH;

    pipe->y_up = 0;
    pipe->height_up = (float)GetRandomValue(PLAYER_HEIGHT, SCREEN_HEIGHT - PIPE_SPACE - PLAYER_HEIGHT);

    pipe->y_down =  pipe->height_up + PIPE_SPACE;
    pipe->height_down = SCREEN_HEIGHT;

    pipe->score_consumed = false;
}

void init_pipes(struct Pipes * pipes) {
    if (pipes == NULL)
        return;

    for (int i = 0; i < pipes->count; i++) {
        init_pipe(&pipes->array[i], -PIPE_WIDTH + PIPE_WIDTH * 7 * (pipes->count - i - 1));

        if (i == 0)
            pipes->last_pipe_offset = -PIPE_WIDTH + PIPE_WIDTH * 7 * (pipes->count - 1);
    }
}

void init_player(struct Player * player) {
    if (player == NULL)
        return;

    *player = (struct Player) {
        .rect.x = SCREEN_WIDTH / 2.0 - PLAYER_WIDTH,
        .rect.y = SCREEN_HEIGHT / 2.0,
        .rect.width = PLAYER_WIDTH,
        .rect.height = PLAYER_HEIGHT,
        .jump_delay = 0.05f,
        .last_jump_time = 0,
        .jump_force = 150,
        .can_jump = true,
        .jump_iterations_left = 0,
        .texture_falling = player->texture_falling,
        .texture_jumping = player->texture_jumping,
        .texture_slow_falling = player->texture_slow_falling,
    };
}

void init_game_info(struct GameInfo * game_info) {
    if (game_info == NULL)
        return;

    game_info->score = 0;
    game_info->state = AWAITING_INPUT;
}