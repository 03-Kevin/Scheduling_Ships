#ifndef CALENDARIZADOR_H
#define CALENDARIZADOR_H

#include <stdio.h>
#include <stdlib.h>

#include "../ce_threads/ce_thread.h"

extern int scheduling_type; // Global variable for scheduling type

typedef struct ReadyQueueNode
{
    CEthread *thread; // Use a pointer to CEthread
    struct ReadyQueueNode *next;
} ReadyQueueNode;

typedef struct
{
    ReadyQueueNode *head;
    int count;
} ReadyQueue;

extern ReadyQueue *queue;
extern int scheduling_type;

// Ready Queue management functions
ReadyQueue *create_ready_queue();
void enqueue_thread(CEthread *thread); // Use a pointer to CEthread
CEthread *dequeue_thread();            // Return a pointer to CEthread
void remove_thread_at(int position);
void update_ready_queue();

// Scheduling algorithms
void sort_by_priority();
void sort_by_sjf();
void sort_by_fcfs();

// Scheduling function
void calendar();

void print_ready_queue();

#endif // CALENDARIZADOR_H
