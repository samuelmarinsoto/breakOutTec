#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "constants.h"

//------------------Seccion de definicion de estructuras a usar y otros elementos previo al inicio del juego----------------------
//Estructura que define las dimensiones que deben de tener todos los bloques. Crea el estandar del resto de bloques: Fabrica de bloques.
typedef struct {
   Rectangle rect;
    float w; //Tamaño en 'x' que tiene el bloque a crear
    float h; //Tamaño en 'y' que tiene el bloque a crear
} Brick_factory;

//Estructura que define los datos especificos de un bloque, es decir, es la estructura principal de cada bloque.
typedef struct {
    Brick_factory base; //Ancla el bloque creado con la base que debe tener cada bloque definido en el factory.
    Color color; //Define el color que tiene el bloque el cual es definido luego.
} Brick;

//Estructura que crea la lista dinamica de bloques con las que se almacenan todos los bloques creados y printeados en la pantalla.
typedef struct {
    Brick *data; //Define mediante un puntero el bloque especifico que esta almacenado en la casilla revisada. Este se guarda porque se tienen varios bloques que pueden ser distintos.
    size_t size; //Define el total de blques que aun quedan en la lista de bloques.
    size_t capacity; //Define el total de bloques que se pueden almacenar en la lista.
} BrickArray;


//Estructura de la bola y sus caracteristicas
struct Ball {
    Vector2 pos; //Define la posicion en la que se encuentra la bola actualmente
    Vector2 accel; //Define la direccion en ambos ejes en la que se dirige la bola.
    float vel; //Define la velocidad de la bola.
    float r; //Define el radio que va a tener la bola.
};

//Estructura con el jugador y sus caracteristicas
struct Player {
    Rectangle rect; //Define las dimensiones del rectangulo de la raqueta.
    float velocity; //Define la velocidad de la raqueta.
    int score; //Define la puntuacion que tiene el jugador.
    int lives; //Define las vidas que tiene el jugador.
    int level; //Define el nivel de dificultad en el que se encuentra el jugador.
    float w; //Define el tamaño en el eje 'x' que va a tener la raqueta.
    float h; //Define el tamaño en el eje 'y' que va a tener la raqueta.
};

//Declaraciones antes de la ejecucion del juego.

Texture2D background_text; //Textura del fondo del juego
struct Player player; //Declaracion de un jugador
struct Ball ball; //Declaracion de una jugador
BrickArray bricks; //Declaracion de una lista de bloques dinamica
bool gg = false; //Declaracion de condicion de derrota como booleano

//------------------Seccion de creacion de bloques----------------------
void Spawn_bricks(BrickArray *brick_array) {
    Brick new_brick; //Crea un nuevo bloque para la lista
    new_brick.base.w = default_brick_w; //Le da el valor a la longitud del bloque.
    new_brick.base.h = default_brick_h; //Le da el valor a la altura del bloque.
    brick_array->size = 0; // Resetea cuantos bloques quedan todavia en la lista de bloques dinamica.

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) { //Los for recorren filas y columnas para asi poder crear los bloques en las distintas posiciones.
            new_brick.base.rect.x = 40 + (i * 55); //Acomoda la posicion del nuevo bloque en 'x'
            new_brick.base.rect.y = 50 + (j * 26); //Acomoda la posicion del nuevo bloque en 'y'
            new_brick.base.rect.width = new_brick.base.w; //Define la longitud del nuevo bloque creado
            new_brick.base.rect.height = new_brick.base.h; //Define la altura del nuevo bloque creado

            if (j < 2) { //Asigna el color rojo a las primeras 2 filas de la matriz
                new_brick.color = RED;
            } else if (j < 4) { //Asigna el color naranja a las filas 3 y 4 de la matriz
                new_brick.color = ORANGE;
            } else if (j < 6) { //Asigna el color amarillo a las filas 5 y 6 de la matriz
                new_brick.color = YELLOW;
            } else { //Asigna el color verde a las ultimas 2 filas de la matriz
                new_brick.color = GREEN;
            }

            brick_array->data[brick_array->size++] = new_brick; //Se indica que hubo un aumento en el total de bloques de la lista.
        }
    }
}

//------------------Seccion de cargado de los elementos necesarios en el juego----------------------
void Game_startup(BrickArray *brick_array) {

    //Codigo que se encarga de cargar el fondo del juego en la memoria para mas adelante proyectarlo.
    Image background_img = LoadImage("../assets/Space.png");
    background_text = LoadTextureFromImage(background_img);
    UnloadImage(background_img);

    //Codigo que carga a memoria datos del jugador
    player.rect = (Rectangle) {default_rectpos_x, default_rectpos_y, default_racket_w, default_racket_h}; //Carga la posicion y las dimensiones iniciales del rect
    player.velocity = default_racket_velocity; //Carga la velocidad inicial del jugador
    player.score = 0; //Carga la puntuacion del jugador
    player.w = default_racket_w; //Carga la longitud inicial del jugador
    player.h = default_racket_h; //Carga la altura del jugador
    player.lives = default_racket_lives; //Carga las vidas que tiene disponible el jugador
    player.level = default_level; //Carga el nivel inicial al que se encuentra el jugador

    //Codigo que carga a memoria los datos base que debe tener la bola justo antes de que se inicie la partida.
    ball.accel = (Vector2) {default_accel_x, default_accel_y}; //Carga la direccion a la que se dirige la bola inicialmente.
    ball.r = default_r; //Carga el radio que tiene la bola
    ball.pos = (Vector2) {default_ballpos_x, default_ballpos_y}; //Carga la posicion inicial de la bola.
    ball.vel = default_ball_velocity; //Carga la velocidad inicial de la bola.

    //Codigo que carga la lista de bloques
    brick_array->size = 0; //Define en 0 la cantidad de bloques iniciales
    brick_array->capacity = ba_size; // Capacidad total que va a tener la lista de bloques ya seteada.
    brick_array->data = (Brick *)malloc(brick_array->capacity * sizeof(Brick)); //Carga en memoria con la ayuda de punteros los lugares a los que se van a guardar los bloques.

    Spawn_bricks(brick_array); //Declaracion para que se creen los bloques

}

//------------------Seccion de elementos chequeados a cada rato en el juego----------------------
void Game_update() {

    float framet = GetFrameTime(); //Obtener el frametime del juego


    //Verifica fin de la partida
    if (gg) return;

    //Control del jugador sobre la barra de juego.
    if(IsKeyDown(KEY_LEFT)) {
        player.rect.x -= player.velocity * framet; //Control con la direccional izquierda para moverse a la izquierda.
    }
    if(IsKeyDown(KEY_RIGHT)) {
        player.rect.x += player.velocity * framet; //Control con la direccional derecha para moverse a la derecha.
    }

    //Actualizacion de la posicion de la bola constante en 'x' y en 'y'
    ball.pos.x = ball.pos.x + ((ball.vel * ball.accel.x) * framet);
    ball.pos.y = ball.pos.y + ((ball.vel * ball.accel.y) * framet);

    //------------------Seccion de colisiones y otras interacciones del juego----------------------

    //Colision entre la bola y los bloques.
    for (int i = 0; i < bricks.size; i++) {
        Brick brick = bricks.data[i];
        if (CheckCollisionCircleRec( //Va chequeando si se detecta una colision con un bloque.
            ball.pos,
            ball.r,
            brick.base.rect
        )) {
            ball.accel.y = ball.accel.y * -1; //Cambia la direccion de la bola
            player.score = player.score + 10; // Actualiza el puntaje del juego
            for (int j = i; j < bricks.size - 1; j++) {
                bricks.data[j] = bricks.data[j + 1];
            }

            bricks.size--;
            i--;
            break;
        }
    }

    //Chequeo de si todos los bloques estan destruidos, si ese es el caso, se aumenta el nivel, se reestablecen los bloques y se aumenta la velocidad de la bola.
    if (bricks.size == 0) {
        player.level++; //Aumento de nivel
        ball.vel *= 1.2f; //Se aumenta la velocidad de la bola.
        ball.accel = (Vector2) {default_accel_x, default_accel_y}; //Se vuelve a la aceleracion de la bola original
        ball.pos = (Vector2) {default_ballpos_x, default_ballpos_y}; //Se vuelve a la posicion original de la bola
        Spawn_bricks(&bricks); //Se vuelven a generar bloques
    }

    //Colision entre la bola y las paredes, se invierte la aceleracion pues el choque causa cambio a direccion contraria.
    if (ball.pos.x > screen_w || ball.pos.x < 10) {
        ball.accel.x = ball.accel.x * -1; //Cambia el eje 'x' al que va la bola al contrario
    }
    if (ball.pos.y < 10) {
        ball.accel.y = ball.accel.y * -1; //Cambia el eje 'y' al que va la bola al contrario
    }

    //Chequeo de si la bola se va de la pantalla abajo para posteriormente volver a jugar pero con una vida menos.
    if (ball.pos.y > screen_h) {
        player.lives--; //Quita una vida
        ball.pos = (Vector2){default_ballpos_x, default_ballpos_y}; //Se vuelve a la posicion de la bola original
        ball.accel = (Vector2){default_accel_x, default_accel_y}; //Se vuelve a la aceleracion de la bola original
        if (player.lives <= 0) {
            gg = true; //Se activa la condicion para acabar con la partida.
        }
        return;
    }


    //Colision entre la bola y el jugador.
    if (CheckCollisionCircleRec(ball.pos, ball.r, player.rect)) {
        //Se genera un numero aleatorio y se le saca modulo 2.
        //Esto causa un escenario 50/50 para decidir la direccion de la bola una vez golpea la raqueta aleatoriamente.
        ball.accel.x = (rand() % 2 == 0 ? 1 : -1) * ball.accel.x;
        ball.accel.y = ball.accel.y * -1;
    }

    //Colision entre el jugador y las paredes
    if (player.rect.x < 0) { //Chequeo por si choca con la parte izquierda de la pantalla.
        player.rect.x = 0;
    }
    if (player.rect.x > (screen_w - player.rect.width)) { //Chequeo por si choca con la parte derecha de la pantalla.
        player.rect.x = (screen_w - player.rect.width);
    }


}

//------------------Seccion de renderizacion----------------------

void Game_render() {

    //Codigo que renderiza el fondo, se expande la imagen original para que se vea en toda la pantalla.
    Rectangle source = {0,0,background_text.width, background_text.height}; //Detalles de donde sale el fondo original.
    Rectangle dest = {0,0,screen_h,screen_h}; //Dimensiones del rect del fondo y su posicion
    Vector2 origin = {0,0}; //Inicio del fondo

    DrawTexturePro(background_text, source, dest, origin, 0.0f, RAYWHITE); //Dibuja en el fondo de manera custom.

    //Codigo que renderiza los bloques del Breakout.
    for (size_t i = 0; i < bricks.size; i++) { //Recore cada una de las posiciones de la lista de bloques.
        Brick brick = bricks.data[i];
        DrawRectangle( //Se dibuja el rectangulo teniendo en cuenta los parametros previamente analizados en la construccion del bloque
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
    sprintf(score, "%d", player.score); //Renderiza el puntaje numerico obtenido hasta el momento

    strcat(score_txt, score);
    DrawText(score_txt, 10, 10, 15, RAYWHITE);

    //Codigo que renderiza el puntaje del jugador como si fuese un string en la pantalla.
    char lives_txt[70] = "VIDAS: ";

    char lives[60];
    sprintf(lives, "%d", player.lives); //Renderiza las vidas en valor numerico que quedan hasta el momento

    strcat(lives_txt, lives);
    DrawText(lives_txt, screen_w-75, 10, 15, RAYWHITE);

    //Codigo que renderiza el puntaje del jugador como si fuese un string en la pantalla.
    char level_txt[70] = "NIVEL: ";

    char level[60];
    sprintf(level, "%d", player.level); //Renderiza el nivel en valor numerico en el que el jugador se encuentra hasta el momento

    strcat(level_txt, level);
    DrawText(level_txt, 230, 10, 15, RAYWHITE);

    if (gg) { //Condicion de si se pierde el juego.
        DrawText("HAS PERDIDO: TE QUEDASTE SIN VIDAS", screen_w / 2 - 200, screen_h / 2 - 10, 20, RED);
        DrawText("PRESIONA ESC PARA SALIR", screen_w / 2 - MeasureText("Press R to restart or Q to quit", 15) / 2, screen_h / 2 + 40, 15, RAYWHITE);
        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow(); // Se termina todo
        }
    }

}

void Game_shutdown() { //Se libera toda la data almacenada sobre los bloques en la lista de bloques

    free(bricks.data);

}

int main(void) {

    InitWindow(screen_w, screen_h, "breakOutTec"); //Se carga la pantalla del juego

    SetTargetFPS(60); //Se setean los FPS

    srand((unsigned int)time(NULL));  //Seteo de random

    Game_startup(&bricks); //Se inicia el juego

    while (!WindowShouldClose()) {

        Game_update(); //Se chequea las condiciones que se pueden actualizar del juego

        BeginDrawing(); //Se dibuja la pantalla
        ClearBackground(BLUE);

        Game_render(); //Se renderizan todos los elementos del juego

        EndDrawing(); //Se termina el dibujo de pantalla
    }

    Game_shutdown(); //Funcion necesaria de llamar antes de cerrar el juego

    CloseWindow(); //Se cierra la ventana
    return 0;
}
