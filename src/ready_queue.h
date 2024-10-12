/*
 * ready_queue.h
 *
 *  Created on: 10/11/2020
 * author: @kevinrodriguez
 */
#ifndef READY_QUEUE_H
#define READY_QUEUE_H

#define MAX_THREADS 100

// Estructura que define un hilo
typedef struct
{
    int id;
    int priority;
    int burst_time;
    int arrival_time;
} Thread;

// Estructura para la cola de listos
typedef struct
{
    Thread threads[MAX_THREADS];
    int count;
    int lock; // Mutex manual para la cola
} ReadyQueue;

// Declaración de la cola de listos en memoria compartida
extern ReadyQueue *queue;

// Funciones para manejar la cola de listos
void init_ready_queue();                                                                    // Inicializa la cola
void add_thread(ReadyQueue *queue, int id, int priority, int burst_time, int arrival_time); // Agrega un hilo
Thread remove_thread(ReadyQueue *queue);                                                    // Quita un hilo
void print_ready_queue(ReadyQueue *queue);                                                  // Imprime la cola

// Funciones para el mutex manual
void lock(int *mutex);   // Bloquea el acceso
void unlock(int *mutex); // Desbloquea el acceso

#endif // READY_QUEUE_H
