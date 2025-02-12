//
// Created by Antonie Gabriel Belu on 12.02.2025.
//

#include "engine_physics.h"
#include "engine_generate.h"
#include <stddef.h>
#include <raymath.h>

#include "engine_audio.h"

void update_player(struct Player * player, struct GameInfo * game_info) {
    if (player == NULL || game_info == NULL)
        return;

    if (player->rect.y < 0)
        player->rect.y = 0;
    else if (player->rect.y + player->rect.height > SCREEN_HEIGHT) {
        game_info->state = DIED;
        play_sound(DEATH);
    }
}

void update_pipe(struct Pipe * pipe, const int last_pipe_offset, struct GameInfo * game_info) {
    if (pipe == NULL || game_info == NULL)
        return;

    if (!pipe->score_consumed && pipe->x < SCREEN_WIDTH / 2.0 - PLAYER_WIDTH - PIPE_WIDTH) {
        pipe->score_consumed = true;
        game_info->score += 1;

        play_sound(SCORE);
    }

    if (pipe->x < -pipe->width)
        init_pipe(pipe, last_pipe_offset);
}

void update_pipes(struct Pipes * pipes, struct GameInfo * game_info) {
    if (pipes == NULL || game_info == NULL)
        return;

    for (int i = 0; i < pipes->count; i++) {
        pipes->array[i].x -= Clamp(PIPE_MOVEMENT * (float)game_info->score / 25.f, PIPE_MOVEMENT, PIPE_MOVEMENT * 1.5f);
        update_pipe(&pipes->array[i], pipes->last_pipe_offset, game_info);
    }
}

void apply_gravity(struct Player * player, const struct GameInfo * game_info) {
    if (player == NULL || game_info == NULL)
        return;

    if (player->small_gravity_iterations) {
        player->rect.y += Clamp(GRAVITY_FORCE * (float)game_info->score / 35.f, GRAVITY_FORCE / 2.f, GRAVITY_FORCE * 1.f);
        --player->small_gravity_iterations;
    }
    else
        player->rect.y += Clamp(GRAVITY_FORCE * (float)game_info->score / 25.f, GRAVITY_FORCE, GRAVITY_FORCE * 1.5f);;
}

bool has_collided(const struct Player * player, const struct Pipe * pipe)  {
    if (player == NULL)
        return false;

    if (CheckCollisionRecs((Rectangle) {
        pipe->x,
        pipe->y_up,
        pipe->width,
        pipe->height_up
    }, player->rect)) return true;

    return CheckCollisionRecs((Rectangle) {
        pipe->x,
        pipe->y_down,
        pipe->width,
        pipe->height_down
    }, player->rect);
}

bool check_collisions(const struct Player * player, const struct Pipes * pipes) {
    if (player == NULL || pipes == NULL)
        return false;

    for (int i = 0; i < pipes->count; i++) {
        // Skip the ones far away
        if (pipes->array[i].x > SCREEN_WIDTH / 2.0 + player->rect.width || pipes->array[i].x + PIPE_WIDTH < -player->rect.x)
            continue;

        if (has_collided(player, &pipes->array[i]))
            return true;
    }

    return false;
}

void handle_smooth_jump(struct Player * player) {
    if (player == NULL)
        return;

    if (player->jump_iterations_left) {
        // first is big jump
        if (player->jump_iterations_left == JUMP_ITERATIONS)
            player->rect.y -= JUMP_FORCE * 2.f / JUMP_ITERATIONS;
        else
            player->rect.y -= JUMP_FORCE * 1.f / JUMP_ITERATIONS;

        --player->jump_iterations_left;
        if (!player->jump_iterations_left)
            player->small_gravity_iterations = GRAVITY_ITERATIONS;
    }
}
void simulate_jump(struct Player * player, const bool play) {
    if (player == NULL)
        return;

    player->jump_iterations_left = JUMP_ITERATIONS;
    if (play)
        play_sound(JUMP);
}