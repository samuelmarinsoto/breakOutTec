#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <json-c/json.h>
#include "constants.h"

//Definicion de ip local y puerto de conexion del juego
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int extra_life_brick_indexes[64];  // Lista que almacena los diferentes bloques en la lista de bloques que tienen el poder de vida extra
int increase_speed_brick_indexes[64];  // Lista que almacena los diferentes bloques en la lista de bloques que tienen el poder de aumentar la velocidad de la bola
int decrease_speed_brick_indexes[64];  // Lista que almacena los diferentes bloques en la lista de bloques que tienen el poder de reducir la velocidad de la bola
int double_racket_indexes[64]; //Lista que almacena los diferentes bloques en la lista de bloques que tienen el poder de duplicar el tamaño de la raqueta
int half_racket_indexes[64]; //Lista que almacena los diferentes bloques en la lista de bloques que tienen el poder de dividir a la mitad el tamaño de la raqueta
int add_ball_indexes[64]; //Lista que almacena los diferentes bloques en la lista de bloques que tienen el poder de aumentar una bola

int extra_life_count = 0;  // Contador de la cantidad de bloques que tienen el poder de extra vida
int increase_speed_count = 0;  // Contador de la cantidad de bloques que tienen el poder de mas velocidad en la bola
int decrease_speed_count = 0;  // Contador de la cantidad de bloques que tienen el poder de menos velocidad en la bola
int double_racket_count = 0; // Contador de la cantidad de bloques que tienen el poder de duplicar el tamaño de la raqueta
int half_racket_count = 0; // Contador de la cantidad de bloques que tienen el poder de dividir a la mitad el tamaño de la raqueta
int add_ball_count = 0; // Contador de la cantidad de bloques que tienen el poder de agregar una bola


// Funcion que convierte un string con el nombre de un color a un color
Color GetColorFromName(const char *color_name) {
    if (strcmp(color_name, "red") == 0) return RED;
    if (strcmp(color_name, "green") == 0) return GREEN;
    if (strcmp(color_name, "yellow") == 0) return YELLOW;
    if (strcmp(color_name, "orange") == 0) return ORANGE;
}

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
    int score; //Puntaje que da el bloque especifico
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
struct Ball ball; //Declaracion de un jugador
struct Ball balls[10];  // Declaracion de bolas
int num_balls = 1;
BrickArray bricks; //Declaracion de una lista de bloques dinamica
bool gg = false; //Declaracion de condicion de derrota como booleano

//------------------Seccion de comunicacion mediante JSONs y sockets----------------------

int create_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0); //Crea el socket para la recepcion de mensajes
    if (sock < 0) { //Chequeo de si se creo el socket correctamente
        perror("Socket fallo en crearse");
        exit(EXIT_FAILURE);
    }
    return sock; //Da el socket creado
}

void connect_to_server(int sock) {
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("La conexion con el server ha fallado");
        exit(EXIT_FAILURE);
    }
}

void receive_json(int sock, char *buffer, size_t buffer_size) {
    int bytes_received = recv(sock, buffer, buffer_size - 1, 0);
    if (bytes_received < 0) {
        perror("No se recibieron los datos correctamente");
        exit(EXIT_FAILURE);
    }
    buffer[bytes_received] = '\0';
}

void handle_json_message(const char *json_string) {
    struct json_object *parsed_json;
    struct json_object *brick_index;
    struct json_object *extra_life;
    struct json_object *increase_ball_speed;
    struct json_object *decrease_ball_speed;
    struct json_object *double_size_racket;
    struct json_object *half_size_racket;
    struct json_object *increase_ball;
    struct json_object *color;
    struct json_object *score;

    parsed_json = json_tokener_parse(json_string);
    if (!parsed_json) {
        printf("Error parseando el JSON\n");
        return;
    }

    json_object_object_get_ex(parsed_json, "brick_index", &brick_index);
    json_object_object_get_ex(parsed_json, "extra_life", &extra_life);
    json_object_object_get_ex(parsed_json, "increase_ball_speed", &increase_ball_speed);
    json_object_object_get_ex(parsed_json, "decrease_ball_speed", &decrease_ball_speed);
    json_object_object_get_ex(parsed_json, "double_racket", &double_size_racket);
    json_object_object_get_ex(parsed_json, "half_racket", &half_size_racket);
    json_object_object_get_ex(parsed_json, "add_ball", &increase_ball);
    json_object_object_get_ex(parsed_json, "color", &color);
    json_object_object_get_ex(parsed_json, "score", &score);

    int index = json_object_get_int(brick_index);
    bool extraLife = json_object_get_boolean(extra_life);
    bool increaseSpeed = json_object_get_boolean(increase_ball_speed);
    bool decreaseSpeed = json_object_get_boolean(decrease_ball_speed);
    bool doubleRacket = json_object_get_boolean(double_size_racket);
    bool halfRacket = json_object_get_boolean(half_size_racket);
    bool addBall = json_object_get_boolean(increase_ball);
    const char *color_str = json_object_get_string(color);
    int updated_score = json_object_get_int(score);

    for (int i = 0; i < bricks.size; i++) {
        Brick *brick = &bricks.data[i];

        // Compare block color with the received color
        if ((color_str != NULL) && (brick->color.r == GetColorFromName(color_str).r) &&
            (brick->color.g == GetColorFromName(color_str).g) &&
            (brick->color.b == GetColorFromName(color_str).b)) {
            brick->score = updated_score;
            printf("Se puede cambiar el puntaje del color, nuevo puntaje del bloque: %d\n", updated_score);
            }
    }

    if (extraLife && extra_life_count < ba_size) {
        extra_life_brick_indexes[extra_life_count++] = index;
    }

    if (increaseSpeed && increase_speed_count < ba_size) {
        increase_speed_brick_indexes[increase_speed_count++] = index;
    }

    if (decreaseSpeed && decrease_speed_count < ba_size) {
        decrease_speed_brick_indexes[decrease_speed_count++] = index;
    }

    if (doubleRacket && double_racket_count < ba_size) {
        double_racket_indexes[double_racket_count++] = index;
    }

    if (halfRacket && half_racket_count < ba_size) {
        half_racket_indexes[half_racket_count++] = index;
    }

    if (addBall && add_ball_count < ba_size) {
        add_ball_indexes[add_ball_count++] = index;
    }

    json_object_put(parsed_json);
}





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
                new_brick.score = default_red;
            } else if (j < 4) { //Asigna el color naranja a las filas 3 y 4 de la matriz
                new_brick.color = ORANGE;
                new_brick.score = default_orange;
            } else if (j < 6) { //Asigna el color amarillo a las filas 5 y 6 de la matriz
                new_brick.color = YELLOW;
                new_brick.score = default_yellow;
            } else { //Asigna el color verde a las ultimas 2 filas de la matriz
                new_brick.color = GREEN;
                new_brick.score = default_green;
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
    num_balls = 1;  // Set the initial number of balls
    balls[0].pos = (Vector2) {default_ballpos_x, default_ballpos_y}; // Initial position
    balls[0].accel = (Vector2) {default_accel_x, default_accel_y}; // Initial direction
    balls[0].vel = default_ball_velocity; // Initial velocity
    balls[0].r = default_r;

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
    for (int b = 0; b < num_balls; b++) {
        balls[b].pos.x += balls[b].vel * balls[b].accel.x * framet;
        balls[b].pos.y += balls[b].vel * balls[b].accel.y * framet;
    }

    //------------------Seccion de colisiones y otras interacciones del juego----------------------

    //Colision entre la bola y los bloques.
    for (int i = 0; i < bricks.size; i++) {
        Brick *brick = &bricks.data[i];
        for (int b = 0; b < num_balls; b++) {
            if (CheckCollisionCircleRec( //Va chequeando si se detecta una colision con un bloque.
                balls[b].pos,
                balls[b].r,
                brick->base.rect
            )) {
                balls[b].accel.y *= -1; //Cambia la direccion de la bola
                player.score += brick->score; // Actualiza el puntaje del juego

                //Manejo de poderes enviados por el server:

                for (int j = 0; j < extra_life_count; j++) {
                    if (i == extra_life_brick_indexes[j]) {
                        player.lives += 1;
                        printf("Se ha obtenido una vida extra. Vidas: %d\n", player.lives);
                        break; //
                    }
                }

                // Check for increase speed power-ups
                for (int j = 0; j < increase_speed_count; j++) {
                    if (i == increase_speed_brick_indexes[j]) {
                        ball.vel *= 1.1f;  // Increase speed by 10%
                        printf("La velocidad de la bola ha subido. Velocidad actual: %.2f\n", ball.vel);
                        break;
                    }
                }

                // Check for decrease speed power-ups
                for (int j = 0; j < decrease_speed_count; j++) {
                    if (i == decrease_speed_brick_indexes[j]) {
                        ball.vel *= 0.9f;
                        printf("La velocidad de la bola ha bajado. Velocidad actual: %.2f\n", ball.vel);
                        break;
                    }
                }

                for (int j = 0; j < double_racket_count; j++) {
                    if (i == double_racket_indexes[j]) {
                        player.rect.width *= 2;
                        printf("Se duplico el tamaño de la raqueta. Nuevo ancho: %.2f\n", player.rect.width);
                        break;
                    }
                }

                for (int j = 0; j < half_racket_count; j++) {
                    if (i == half_racket_indexes[j]) {
                        player.rect.width *= 0.5f;
                        printf("Se dividio a la mitad el tamaño de la raqueta. Nuevo ancho: %.2f\n", player.rect.width);
                        break;
                    }
                }

                for (int j = 0; j < add_ball_count; j++) {
                    if (i == add_ball_indexes[j]) {

                        if (num_balls < max_balls) {
                            balls[num_balls].pos = (Vector2) {ball.pos.x, ball.pos.y}; // Start new ball at same position
                            balls[num_balls].accel = (Vector2) {default_accel_x, default_accel_y}; // Set the direction
                            balls[num_balls].vel = ball.vel; // Set the same speed as the original ball
                            balls[num_balls].r = ball.r; // Same radius
                            num_balls++; // Increment the ball count
                            printf("Se ha añadido una nueva bola.\n");
                        }
                        break;
                    }
                }

                for (int j = i; j < bricks.size - 1; j++) { //Se elimina el bloque de la lista.
                    bricks.data[j] = bricks.data[j + 1];
                }

                bricks.size--;
                i--;
                break;
            }
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
    for (int b = 0; b < num_balls; b++) {
        if (balls[b].pos.x > screen_w || balls[b].pos.x < 10) {
            balls[b].accel.x *= -1; // Se invierte la direccion en el eje 'x'
        }
        if (balls[b].pos.y < 10) {
            balls[b].accel.y *= -1; // Se invierte la direccion en el eje 'y'
        }

    //Chequeo de si la bola se va de la pantalla abajo para posteriormente volver a jugar pero con una vida menos.
        if (balls[b].pos.y > screen_h) {
            player.lives--; // Pierde una vida el jugador
            balls[b].pos = (Vector2) {default_ballpos_x, default_ballpos_y}; // Reinicia la posicion de la bola
            balls[b].accel = (Vector2) {default_accel_x, default_accel_y}; // Reinicia la aceleracion de la bola
            if (player.lives <= 0) {
                gg = true; // Condicion de si se termina el juego, todas las vidas se perdieron
            }
        }
    }


    //Colision entre la bola y el jugador.
    for (int b = 0; b < num_balls; b++) {
        //Se genera un numero aleatorio y se le saca modulo 2.
        //Esto causa un escenario 50/50 para decidir la direccion de la bola una vez golpea la raqueta aleatoriamente.
        if (CheckCollisionCircleRec(balls[b].pos, balls[b].r, player.rect)) {
            balls[b].accel.x = (rand() % 2 == 0 ? 1 : -1) * balls[b].accel.x;
            balls[b].accel.y *= -1;
        }
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
    for (int b = 0; b < num_balls; b++) {
        DrawCircle(balls[b].pos.x, balls[b].pos.y, balls[b].r, RAYWHITE);
    }

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
