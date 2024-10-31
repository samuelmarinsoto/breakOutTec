#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>
#include <time.h>
#include <math.h>

//Variables que contienen las dimensiones de la pantalla del juego.
const int screen_w = 500;
const int screen_h = 600;

typedef struct {
   Rectangle rect;
    float w;
    float h;
} Brick_factory;

typedef struct {
    Brick_factory base;
    Color color;
} Brick;

typedef struct {
    Brick *data;
    size_t size;
    size_t capacity;
} BrickArray;


//Struct con la bola y sus caracteristicas
struct Ball {
    Vector2 pos;
    Vector2 accel;
    float vel;
    float r;
};

//Struct con el jugador y sus caracteristicas
struct Player {
    Rectangle rect;
    float velocity;
    int score;
    int lives;
    int level;
    float w;
    float h;
};

typedef struct {
    float time;
    float duration;
    bool active;
} Timer;

Texture2D background_text;
struct Player player;
struct Ball ball;
BrickArray bricks;
bool gg = false;

void Spawn_bricks(BrickArray *brick_array) {
    Brick new_brick;
    new_brick.base.w = 50.0f;
    new_brick.base.h = 15.0f;
    brick_array->size = 0; // Reset size for respawning

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            new_brick.base.rect.x = 40 + (i * 55);
            new_brick.base.rect.y = 50 + (j * 26);
            new_brick.base.rect.width = new_brick.base.w;
            new_brick.base.rect.height = new_brick.base.h;

            if (j < 2) {
                new_brick.color = RED;
            } else if (j < 4) {
                new_brick.color = ORANGE;
            } else if (j < 6) {
                new_brick.color = YELLOW;
            } else {
                new_brick.color = GREEN;
            }

            brick_array->data[brick_array->size++] = new_brick;
        }
    }
}

void Game_startup(BrickArray *brick_array) {

    //Codigo que se encarga de cargar el fondo del juego en la memoria para mas adelante proyectarlo.
    Image background_img = LoadImage("../assets/Space.png");
    background_text = LoadTextureFromImage(background_img);
    UnloadImage(background_img);

    //Codigo que carga a memoria datos del jugador
    player.rect = (Rectangle) {250.0f, 540.0f, 75.0f, 10.0f};
    player.velocity = 450.0f;
    player.score = 0;
    player.w = 75.0f;
    player.h = 10.0f;
    player.lives = 3;
    player.level = 1;

    //Codigo que carga a memoria datos de la bola
    ball.accel = (Vector2) {1.0f, -1.0f};
    ball.r = 5.0f;
    ball.pos = (Vector2) {350, 500};
    ball.vel = 300.0f;

    //Codigo que carga la lista de bloques
    brick_array->size = 0;
    brick_array->capacity = 64; // Initial capacity (adjust as needed)
    brick_array->data = (Brick *)malloc(brick_array->capacity * sizeof(Brick));

    Spawn_bricks(brick_array);

}

void Game_update() {

    float framet = GetFrameTime();

    if (gg) return;

    //Control del jugador sobre la barra de juego.
    if(IsKeyDown(KEY_LEFT)) {
        player.rect.x -= player.velocity * framet;
    }
    if(IsKeyDown(KEY_RIGHT)) {
        player.rect.x += player.velocity * framet;
    }

    //Actualizacion de la posicion de la bola
    ball.pos.x = ball.pos.x + ((ball.vel * ball.accel.x) * framet);
    ball.pos.y = ball.pos.y + ((ball.vel * ball.accel.y) * framet);

    //------------------Seccion de colisiones y otras interacciones del juego----------------------

    //Colision entre la bola y los bloques.
    for (int i = 0; i < bricks.size; i++) {
        Brick brick = bricks.data[i];
        if (CheckCollisionCircleRec(
            ball.pos,
            ball.r,
            brick.base.rect
        )) {
            ball.accel.y = ball.accel.y * -1;
            player.score = player.score + 10;
            for (int j = i; j < bricks.size - 1; j++) {
                bricks.data[j] = bricks.data[j + 1];
            }

            bricks.size--;
            i--;
            break;
        }
    }

    //Chequeo de si todos los bloues estan destruidos, si ese es el caso, se aumenta el nivel, se reestablecen los bloques y se aumenta la velocidad de la bola.
    if (bricks.size == 0) {
        player.level++;
        ball.vel *= 1.2f;
        ball.accel = (Vector2) {1.0f, -1.0f};
        ball.pos = (Vector2) {350, 500};
        Spawn_bricks(&bricks);
    }

    //Colision entre la bola y las paredes, se invierte la aceleracion pues el choque causa cambio a direccion contraria.
    if (ball.pos.x > screen_w || ball.pos.x < 10) {
        ball.accel.x = ball.accel.x * -1;
    }
    if (ball.pos.y < 10) {
        ball.accel.y = ball.accel.y * -1;
    }

    //Chequeo de si la bola se va de la pantalla abajo para posteriormente volver a jugar pero con una vida menos.
    if (ball.pos.y > screen_h) {
        player.lives--;
        ball.pos = (Vector2){350, 500};
        ball.accel = (Vector2){1.0f, -1.0f};
        if (player.lives <= 0) {
            gg = true;
        }
        return;
    }


    //Colision entre la bola y el jugador.
    if (CheckCollisionCircleRec(ball.pos, ball.r, player.rect)) {
        //Se genera un numero aleatorio y se le saca modulo 2.
        //Esto causa un escenario 50/50 para decidir la direccion aleatoriamente.
        ball.accel.x = (rand() % 2 == 0 ? 1 : -1) * ball.accel.x;
        ball.accel.y = ball.accel.y * -1;
    }

    //Colision entre el jugador y las paredes
    if (player.rect.x < 0) {
        player.rect.x = 0;
    }
    if (player.rect.x > (screen_w - player.rect.width)) {
        player.rect.x = (screen_w - player.rect.width);
    }


}

void Game_render() {

    //Codigo que renderiza el fondo, se expande la imagen original para que se vea en toda la pantalla.
    Rectangle source = {0,0,background_text.width, background_text.height};
    Rectangle dest = {0,0,600,600};
    Vector2 origin = {0,0};

    DrawTexturePro(background_text, source, dest, origin, 0.0f, RAYWHITE);

    //Codigo que renderiza los bloques del Breakout.
    for (size_t i = 0; i < bricks.size; i++) {
        Brick brick = bricks.data[i];
        DrawRectangle(
            brick.base.rect.x,
            brick.base.rect.y,
            brick.base.rect.width,
            brick.base.rect.height,
            brick.color
        );
    }

    //Codigo que renderiza a la bola
    DrawCircle(ball.pos.x, ball.pos.y, ball.r, RAYWHITE);

    //Codigo que renderiza al jugador
    DrawRectangle(player.rect.x, player.rect.y, player.rect.width, player.rect.height, WHITE);

    //Codigo que renderiza el puntaje del jugador como si fuese un string en la pantalla.
    char score_txt[70] = "PUNTAJE: ";

    char score[60];
    sprintf(score, "%d", player.score);

    strcat(score_txt, score);
    DrawText(score_txt, 10, 10, 15, RAYWHITE);

    //Codigo que renderiza el puntaje del jugador como si fuese un string en la pantalla.
    char lives_txt[70] = "VIDAS: ";

    char lives[60];
    sprintf(lives, "%d", player.lives);

    strcat(lives_txt, lives);
    DrawText(lives_txt, screen_w-75, 10, 15, RAYWHITE);

    //Codigo que renderiza el puntaje del jugador como si fuese un string en la pantalla.
    char level_txt[70] = "NIVEL: ";

    char level[60];
    sprintf(level, "%d", player.level);

    strcat(level_txt, level);
    DrawText(level_txt, 230, 10, 15, RAYWHITE);

    if (gg) {
        DrawText("HAS PERDIDO: TE QUEDASTE SIN VIDAS", screen_w / 2 - 200, screen_h / 2 - 10, 20, RED);
        DrawText("PRESIONA ESC PARA SALIR", screen_w / 2 - MeasureText("Press R to restart or Q to quit", 15) / 2, screen_h / 2 + 40, 15, RAYWHITE);
        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow(); // Exit the game
        }
    }

}

void Game_shutdown() {

    free(bricks.data);

}

int main(void) {

    InitWindow(screen_w, screen_h, "breakOutTec");

    SetTargetFPS(60);

    srand((unsigned int)time(NULL));

    Game_startup(&bricks);

    while (!WindowShouldClose()) {

        Game_update();

        BeginDrawing();
        ClearBackground(BLUE);

        Game_render();

        EndDrawing();
    }

    Game_shutdown();

    CloseWindow();
    return 0;
}
