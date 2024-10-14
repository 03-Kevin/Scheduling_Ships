#ifndef CE_THREAD_H
#define CE_THREAD_H

#include <stdio.h>

#define READY 0
#define RUNNING 1
#define DONE 2

#define OCEANO_IZQ 0
#define OCEANO_DER 1

extern int arrival_counter_left;
extern int arrival_counter_right;
extern int join_flag;

// Estructura para representar un hilo (barco)
typedef struct
{
    int thread_id;
    int state; // state del hilo (READY, RUNNING, DONE)
    int original_side;
    int priority;
    int speed;
    int burst_time;
    int arrival_time;
    void (*thread_function)(void *); // Función que ejecutará el hilo
    void *arg;                       // Argumentos para la función del hilo
} CEthread;

// Prototipos de funciones de CEthreads

// Crea un nuevo hilo (barco) y lo pone en la cola de READYs
int CEthread_create(CEthread *thread, int original_side, int priority, int speed, int canal_length, int arrival_time, void (*thread_function)(void *), void *arg);

// Finaliza el hilo (barco) una vez que ha completado su tarea
void CEthread_end(CEthread *thread);

// Espera a que un hilo (barco) termine
void CEthread_join(CEthread *thread);

// Ejecuta la función del barco
void CEthread_execute(CEthread *thread);

// Crea barcos por batch
void CEthread_create_batch(CEthread *threads, int base_number, int num_threads, int speed, int canal_length, int arrival_time, int priority, int original_side, void (*thread_function)(void *), void *arg);

#endif
