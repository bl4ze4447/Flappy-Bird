#include <stdio.h>

#include "engine_audio.h"
#include "engine_draw.h"
#include "engine_generate.h"
#include "engine_physics.h"
#include "engine_types.h"

struct GameInfo game_info = {
    .score = 0,
    .state = AWAITING_INPUT
};

bool handle_input(struct Player *);
bool handle_death(struct Player * player, struct Pipes * pipes, int);
bool handle_awaiting_input(struct Player * player, const struct Pipes * pipes);

int main() {
    struct Player player;
    init_player(&player);

    init_draw_engine(&player);
    init_audio_engine(0.5f);

    struct Pipes pipes;
    pipes.count = sizeof(pipes.array) / sizeof(pipes.array[0]);
    init_pipes(&pipes);

    // Game loop
    int smooth_gravity_iterations = GRAVITY_ITERATIONS / 2;
    while (!WindowShouldClose()) {
        handle_smooth_jump(&player);
        if (handle_death(&player, &pipes, smooth_gravity_iterations)) {
            --smooth_gravity_iterations;
            continue;
        }
        if (handle_awaiting_input(&player, &pipes))
            continue;

        if (handle_input(&player))
            play_sound(JUMP);

        if (!player.jump_iterations_left)
            apply_gravity(&player, &game_info);

        smooth_gravity_iterations = GRAVITY_ITERATIONS / 2;

        update_player(&player, &game_info);
        update_pipes(&pipes, &game_info);

        if (check_collisions(&player, &pipes)) {
            play_sound(DEATH);
            game_info.state = DIED;
        }

        BeginDrawing();
        draw_frame(&player, &pipes, &game_info);
        EndDrawing();
    }

    draw_engine_cleanup(&player);
    audio_engine_cleanup();
    return 0;
}






bool handle_input(struct Player * player) {
    if (player == NULL)
        return false;

    if (player->can_jump && IsKeyDown(KEY_UP) && GetTime() - player->last_jump_time > player->jump_delay) {
        player->can_jump = false;
        player->jump_iterations_left = JUMP_ITERATIONS;
        player->last_jump_time = GetTime();
        return true;
    }

    if (IsKeyReleased(KEY_UP))
        player->can_jump = true;

    return false;
}

bool handle_death(struct Player * player, struct Pipes * pipes, const int smooth_gravity_iterations) {
    if (player == NULL)
        return true;

    if (game_info.state != DIED)
        return false;

    player->rect.y += GRAVITY_FORCE * 1.1f;
    player->small_gravity_iterations = smooth_gravity_iterations; // for fall texture

    static char score_text[256];
    sprintf(score_text, "GAME OVER!\nPress R to Resume!\nScore: %d", game_info.score);

    BeginDrawing();
    draw_frame(player, pipes, &game_info);
    DrawText(score_text, 20, 20, 40, WHITE);
    EndDrawing();

    if (!IsKeyPressed(KEY_R)) return true;

    init_player(player);
    init_pipes(pipes);
    init_game_info(&game_info);
    return true;
}

bool handle_awaiting_input(struct Player * player, const struct Pipes * pipes) {
    if (player == NULL)
        return true;

    if (game_info.state != AWAITING_INPUT)
        return false;

    static const char * info_text = "Press the UP Arrow Key to start";
    const int info_center_x = (SCREEN_WIDTH - MeasureText(info_text, 40)) / 2;

    player->small_gravity_iterations = 1;
    BeginDrawing();
    draw_frame(player, pipes, &game_info);
    DrawText(info_text, info_center_x, 100, 40, GOLD);
    EndDrawing();

    if (IsKeyPressed(KEY_UP)) {
        game_info.state = RUNNING;
        return false;
    }

    return true;
}