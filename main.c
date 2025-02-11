#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SCREEN_WIDTH        1280
#define SCREEN_HEIGHT       720
#define PLAYER_HEIGHT       30
#define PLAYER_WIDTH        50
#define GRAVITY_FORCE       6       // How many pixels does the player drop per frame
#define PIPE_WIDTH          150
#define PIPE_SPACE          (PLAYER_HEIGHT * 7)
#define PIPE_MOVEMENT       10      // How fast the pipes move towards the left side of the screen
#define JUMP_FORCE          70      // How many pixels does the player jump
#define JUMP_ITERATIONS     10      // divided in this many steps for a visually smoother jump
                                    // First iteration is doubled, making it JUMP_FORCE + (JUMP_FORCE/JUMP_ITERATIONS)

/* ----------- GAME STRUCTS/ENUMS ----------- */
enum GameState {
    RUNNING,
    DIED,
};

struct GameInfo {
    int score;
    enum GameState state;
} game_info;

struct Player {
    Rectangle   rect;
    Texture2D   texture_falling;
    Texture2D   texture_jumping;

    double      jump_delay;
    double      last_jump_time;
    int         jump_force;
    int         jump_iterations_left;
    bool        can_jump;
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
/* ----------- ----------------- ----------- */

Texture2D top_pipe_texture, bottom_pipe_texture;
int last_pipe_offset;   // Spawn location of pipes that reached the left-side of the screen

/* ----------- FUNCTIONS ----------- */
/* Draw functions also manage the movement of their parameter (excluded: draw_score) */
void draw_player(struct Player *);
void draw_pipe(struct Pipe *);
void draw_pipes(struct Pipe [], int);
void draw_score(int);

/* Handles jump input */
void handle_input(struct Player *);

bool has_collided(const struct Player *, const struct Pipe *);
bool check_collisions(const struct Player *, const struct Pipe [], int);
void apply_gravity(struct Player *);

void generate_pipe(struct Pipe *, int);
void generate_pipes(struct Pipe [], int);
void init_player(struct Player *);

void load_player_texture(struct Player *);
Texture2D load_background_texture(void);
void load_pipe_textures(void);

bool handle_death(struct Player * player, struct Pipe pipes[], int pipe_count, const Texture2D * bg_texture);
/* ----------- --------------- ----------- */

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Birds");
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)GetTime());

    struct Player player;
    struct Pipe pipes[6];
    const int pipe_count = sizeof(pipes) / sizeof(pipes[0]);

    init_player(&player);
    load_player_texture(&player);
    generate_pipes(pipes, pipe_count);
    load_pipe_textures();
    const Texture2D bg_texture = load_background_texture();

    // Game loop
    while (!WindowShouldClose()) {
        if (handle_death(&player, pipes, pipe_count, &bg_texture))
            continue;

        handle_input(&player);
        if (!player.jump_iterations_left)
            apply_gravity(&player);

        if (player.jump_iterations_left) {
            // first is big jump
            player.rect.y -= JUMP_FORCE / (player.jump_iterations_left == JUMP_ITERATIONS ? JUMP_ITERATIONS/2.f : JUMP_ITERATIONS);
            --player.jump_iterations_left;
        }

        const bool collided = check_collisions(&player, pipes, pipe_count);

        BeginDrawing();

        if (collided)
            game_info.state = DIED;

        ClearBackground(WHITE);
        DrawTexture(bg_texture, 0, 0, WHITE);

        draw_pipes(pipes, pipe_count);
        draw_player(&player);
        draw_score(game_info.score);

        DrawFPS(20, 20);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


void draw_player(struct Player * player) {
    if (player == NULL) return;

    if (player->rect.y < 0) player->rect.y = 0;
    else if (player->rect.y + player->rect.height > SCREEN_HEIGHT) {
        game_info.state = DIED;
    }

    if (player->jump_iterations_left)
        DrawTexture(player->texture_jumping, (int)player->rect.x, (int)player->rect.y, WHITE);
    else
        DrawTexture(player->texture_falling, (int)player->rect.x, (int)player->rect.y, WHITE);
}

void draw_pipe(struct Pipe * pipe) {
    if (!pipe->score_consumed && pipe->x < SCREEN_WIDTH / 2.0 - PLAYER_WIDTH - PIPE_WIDTH) {
        pipe->score_consumed = true;
        game_info.score += 1;
    }

    if (pipe->x < -pipe->width)
        generate_pipe(pipe, last_pipe_offset);

    DrawTexture(top_pipe_texture, (int)pipe->x, (int)(pipe->y_up - SCREEN_HEIGHT + pipe->height_up), WHITE);
    DrawTexture(bottom_pipe_texture, (int)pipe->x, (int)pipe->y_down, WHITE);
}

void draw_pipes(struct Pipe pipes[], const int pipe_count) {
    for (int i = 0; i < pipe_count; i++) {
        pipes[i].x -= Clamp(PIPE_MOVEMENT * (float)game_info.score / 10.f, PIPE_MOVEMENT, PIPE_MOVEMENT * 1.5f);
        draw_pipe(&pipes[i]);
    }
}

void draw_score(const int score) {
    static char buf[32];
    sprintf(buf, "%d", score);
    DrawText(buf, SCREEN_WIDTH / 2.0, 30, 50, WHITE);
}

void handle_input(struct Player * player) {
    if (player->can_jump && IsKeyDown(KEY_UP) && GetTime() - player->last_jump_time > player->jump_delay) {
        player->can_jump = false;
        player->jump_iterations_left = JUMP_ITERATIONS;
        player->last_jump_time = GetTime();
    }

    if (IsKeyReleased(KEY_UP))
        player->can_jump = true;
}

bool has_collided(const struct Player * player, const struct Pipe * pipe)  {
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

bool check_collisions(const struct Player * player, const struct Pipe pipes[], const int pipe_count) {
    if (player == NULL)
        return false;

    for (int i = 0; i < pipe_count; i++) {
        // Skip the ones far away
        if (pipes[i].x > SCREEN_WIDTH / 2.0 + player->rect.width || pipes[i].x + PIPE_WIDTH < -player->rect.x)
            continue;

        if (has_collided(player, &pipes[i]))
            return true;
    }

    return false;
}

void apply_gravity(struct Player * player) {
    if (player == NULL)
        return;

    player->rect.y += (float)GRAVITY_FORCE;
}

void generate_pipe(struct Pipe * pipe, const int offset) {
    pipe->x = SCREEN_WIDTH + offset;
    pipe->width = PIPE_WIDTH;

    pipe->y_up = 0;
    pipe->height_up = (float)GetRandomValue(0, SCREEN_HEIGHT - PIPE_SPACE);

    pipe->y_down =  pipe->height_up + PIPE_SPACE;
    pipe->height_down = SCREEN_HEIGHT;

    pipe->score_consumed = false;
}

void generate_pipes(struct Pipe pipes[], const int pipe_count) {
    for (int i = 0; i < pipe_count; i++) {
        generate_pipe(&pipes[i], PIPE_WIDTH * 7 * (pipe_count - i - 1));

        if (i == 0)
            last_pipe_offset = PIPE_WIDTH * 7 * (pipe_count - 1);
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
        .texture_jumping = player->texture_jumping
    };
}

void load_player_texture(struct Player * player) {
    if (player == NULL)
        return;

    Image player_img = LoadImage("player.png");
    ImageResize(&player_img, PLAYER_WIDTH, PLAYER_HEIGHT);

    ImageRotate(&player_img, -30);
    player->texture_jumping = LoadTextureFromImage(player_img);

    ImageRotate(&player_img, 30);
    player->texture_falling = LoadTextureFromImage(player_img);

    UnloadImage(player_img);
}

Texture2D load_background_texture(void) {
    Image bg_img = LoadImage("background.jpg");
    ImageResize(&bg_img, SCREEN_WIDTH, SCREEN_HEIGHT);
    const Texture2D bg_texture = LoadTextureFromImage(bg_img);
    UnloadImage(bg_img);

    return bg_texture;
}

void load_pipe_textures(void) {
    Image pipe_img = LoadImage("bottom_pipe.png");
    ImageResize(&pipe_img, PIPE_WIDTH, SCREEN_HEIGHT);

    bottom_pipe_texture = LoadTextureFromImage(pipe_img);
    ImageRotate(&pipe_img, 180);
    top_pipe_texture = LoadTextureFromImage(pipe_img);
    UnloadImage(pipe_img);
}

bool handle_death(struct Player * player, struct Pipe pipes[], const int pipe_count, const Texture2D * bg_texture) {
    if (player == NULL || bg_texture == NULL)
        return true;

    if (game_info.state != DIED)
        return false;

    BeginDrawing();
    ClearBackground(WHITE);
    DrawTexture(*bg_texture, 0, 0, WHITE);

    static char score_text[256];
    sprintf(score_text, "GAME OVER!\nPress R to Resume!\nScore: %d", game_info.score);
    DrawText(score_text, 20, 20, 40, WHITE);

    EndDrawing();

    if (IsKeyPressed(KEY_R)) {
        init_player(player);
        generate_pipes(pipes, pipe_count);

        game_info.score = 0;
        game_info.state = RUNNING;
    }

    return true;
}