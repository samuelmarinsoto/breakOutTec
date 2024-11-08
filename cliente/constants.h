#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PORT 12345
#define SERVER_IP "127.0.0.1"

//Constantes que tienen las dimensiones de la pantalla.
const int screen_w = 500; //El ancho de la pantalla de juego
const int screen_h = 600; //La altura de la pantalla de juego

//Constantes con las caracteristicas bases que tiene la creacion de los bloques printeados en la pantalla.

const float default_brick_w = 50.0f; //El ancho de cada bloque
const float default_brick_h = 15.0f; //La altura de cada bloque.

//Constantes con las caracteristicas bases que tiene el jugador/raqueta.

const float default_rectpos_x = 250.0f; //Posicion base donde aparece al principio la raqueta en el eje 'x'.
const float default_rectpos_y = 540.0f; //Posicion base donde aparece al principio la raqueta en el eje 'y'.

const float default_racket_w = 75.0f; //La longitud base que tiene la raqueta/jugador.
const float default_racket_h  = 10.0f; //La altura base de la raqueta/jugador .

const float default_racket_velocity = 450.0f; //La velocidad base que tiene el jugador al iniciar la partida.
const int default_racket_lives = 3; //La cantidad de vidas con las que inicia el jugador la partida.
const int default_level = 1; //El nivel de dificultad con el que se inicia el juego.

//Constantes con las caracteristicas bases que tiene la bola.

const float default_accel_x = 1.0f; //Direccion inicial en donde se mueve la bola en el eje "x"
const float default_accel_y = -1.0f; //Direccion inicial en donde se mueve la bola en el eje "y"

const float default_r = 5.0f; //Radio inicial de la bola que define tambien su tamaño inicial.
const float default_ball_velocity = 10.0f; //Velocidad inicial de la bola.

const float default_ballpos_x = 350; //Posicion inicial en 'x' que tiene la bola al spawnear.
const float default_ballpos_y = 500; //Posicion inicial en 'y' que tiene la bola al spawnear.

//Constante que define el tamaño que tiene la lista dinamica con los bloques a la hora de crearla.
const int ba_size = 64;

//Constante que define el maximo de bolas con las que puede lidiar el juego a la vez.
const int max_balls = 10;

//Constantes que definen los puntajes de cada color de bloques de manera inicial.

const int default_red = 100; //Puntuacion inicial para los bloques rojos
const int default_orange = 75; //Puntuacion inicial para los bloques naranjas
const int default_yellow = 50; //Puntuacion inicial para los bloques amarillos
const int default_green = 25; //Puntuacion inicial para los bloques verdes

#endif //CONSTANTS_H
