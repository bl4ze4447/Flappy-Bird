//
// Created by Antonie Gabriel Belu on 12.02.2025.
//

#include "engine_draw.h"
#include <stddef.h>
#include <stdio.h>

static Texture2D top_pipe_texture, bottom_pipe_texture, background_texture;

void init_draw_engine(struct Player * player) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Birds");
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)GetFrameTime());

    load_background_texture();
    load_pipe_textures();
    load_player_texture(player);
}

void draw_player(const struct Player * player) {
    if (player == NULL)
        return;

    if (player->jump_iterations_left > 0)
        DrawTexture(player->texture_jumping, (int)player->rect.x, (int)player->rect.y, WHITE);
    else if (player->small_gravity_iterations > 0)
        DrawTexture(player->texture_slow_falling, (int)player->rect.x, (int)player->rect.y, WHITE);
    else
        DrawTexture(player->texture_falling, (int)player->rect.x, (int)player->rect.y, WHITE);
}

void draw_pipe(const struct Pipe * pipe) {
    if (pipe == NULL)
        return;

    DrawTexture(top_pipe_texture, (int)pipe->x, (int)(pipe->y_up - SCREEN_HEIGHT + pipe->height_up), WHITE);
    DrawTexture(bottom_pipe_texture, (int)pipe->x, (int)pipe->y_down, WHITE);
}

void draw_pipes(const struct Pipes * pipes) {
    if (pipes == NULL)
        return;

    for (int i = 0; i < pipes->count; i++)
        draw_pipe(&pipes->array[i]);
}

void draw_score(const int score) {
    static char buf[32];
    sprintf(buf, "%d", score);
    DrawText(buf, SCREEN_WIDTH / 2.0, 30, 50, WHITE);
}

void draw_frame(const struct Player * player, const struct Pipes * pipes, const struct GameInfo * game_info) {
    if (game_info == NULL)
        return;

    // Clear previous frame and draw background
    ClearBackground(WHITE);
    DrawTexture(background_texture, 0, 0, WHITE);

    // Draw pipes, then player, then score, score must be on top of everything
    draw_pipes(pipes);
    draw_player(player);
    draw_score(game_info->score);

    DrawFPS(20, 20);
}

void load_player_texture(struct Player * player) {
    if (player == NULL)
        return;

    Image player_img = LoadImage("player.png");
    ImageResize(&player_img, PLAYER_WIDTH, PLAYER_HEIGHT);

    player->texture_slow_falling = LoadTextureFromImage(player_img);

    ImageRotate(&player_img, -30);
    player->texture_jumping = LoadTextureFromImage(player_img);

    ImageRotate(&player_img, 45);
    player->texture_falling = LoadTextureFromImage(player_img);

    UnloadImage(player_img);
}
void unload_player_texture(const struct Player * player) {
    if (player == NULL)
        return;

    UnloadTexture(player->texture_slow_falling);
    UnloadTexture(player->texture_jumping);
    UnloadTexture(player->texture_falling);
}

void load_background_texture(void) {
    Image bg_img = LoadImage("background.jpg");
    ImageResize(&bg_img, SCREEN_WIDTH, SCREEN_HEIGHT);
    background_texture = LoadTextureFromImage(bg_img);
    UnloadImage(bg_img);
}
void unload_background_texture(void) {
    UnloadTexture(background_texture);
}

void load_pipe_textures(void) {
    Image pipe_img = LoadImage("bottom_pipe.png");
    ImageResize(&pipe_img, PIPE_WIDTH, SCREEN_HEIGHT);

    bottom_pipe_texture = LoadTextureFromImage(pipe_img);
    ImageRotate(&pipe_img, 180);
    top_pipe_texture = LoadTextureFromImage(pipe_img);
    UnloadImage(pipe_img);
}
void unload_pipe_textures(void) {
    UnloadTexture(top_pipe_texture);
    UnloadTexture(bottom_pipe_texture);
}

void draw_engine_cleanup(const struct Player * player) {
    unload_background_texture();
    unload_pipe_textures();
    unload_player_texture(player);
    CloseWindow();
}
