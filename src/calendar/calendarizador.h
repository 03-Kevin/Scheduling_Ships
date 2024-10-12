#ifndef CALENDARIZADOR_H
#define CALENDARIZADOR_H

#include <stdio.h>
#include <stdlib.h>

#define READY 0
#define RUNNING 1
#define DONE 2

extern int scheduling_type; // Variable global para el tipo de calendarización

typedef struct
{
    int thread_id;
    int state;
    int original_side;
    int priority;
    int speed;
    int burst_time;
    int arrival_time;
    void (*thread_function)(void *); // Función del hilo
    void *arg;                       // Argumentos para la función del hilo
} CEthread;

typedef struct ReadyQueueNode
{
    CEthread thread;
    struct ReadyQueueNode *next;
} ReadyQueueNode;

typedef struct
{
    ReadyQueueNode *head;
    int count;
} ReadyQueue;

// Funciones de manejo de la cola
ReadyQueue *create_ready_queue();
void enqueue_thread(CEthread thread);
CEthread dequeue_thread();
void remove_thread_at(int position);
void update_ready_queue();

// Algoritmos de calendarización
void sort_by_priority();
void sort_by_sjf();
void sort_by_fcfs();

// Función de calendarización
void calendar();

void print_ready_queue();


#endif // CALENDARIZADOR_H
