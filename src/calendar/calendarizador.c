/**
 * @file calendarizador.c
 * @author Kevin (kjrodriguez@estudiantec.cr)
 * @brief 
 * @version 0.1
 * @date 2024-10-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "calendarizador.h"

ReadyQueue *queue = NULL;  // Cola global
int scheduling_type = 0;   // Tipo de calendarización global

/**
 * @brief Create a ready queue object
 * 
 * @return ReadyQueue* 
 */
ReadyQueue *create_ready_queue()
{
    queue = (ReadyQueue *)malloc(sizeof(ReadyQueue));
    queue->head = NULL;
    queue->count = 0;
    return queue;
}
/**
 * @brief enqueue_thread: agregar un hilo a la cola de listos, además de actualizar la cola automáticamente.
 * 
 * @param thread 
 */
void enqueue_thread(CEthread thread)
{
    ReadyQueueNode *new_node = (ReadyQueueNode *)malloc(sizeof(ReadyQueueNode));
    new_node->thread = thread;
    new_node->next = NULL;

    if (queue->head == NULL)
    {
        queue->head = new_node;
    }
    else
    {
        ReadyQueueNode *temp = queue->head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
    queue->count++;

    // Actualizar cola automáticamente
    update_ready_queue();
}
/**
 * @brief dequeue_thread: extraer un hilo de la cola de listos. Siempre se extrae el primer hilo de la cola. 
 * Lo extrae, es decir, lo elimina de la cola de listos.
 * 
 * @return CEthread 
 */
CEthread dequeue_thread()
{
    if (queue->head == NULL)
    {
        return (CEthread){0}; // Cola vacía
    }

    ReadyQueueNode *temp = queue->head;
    CEthread thread = temp->thread;
    queue->head = queue->head->next;
    queue->count--;
    free(temp);

    return thread;
}
/**
 * @brief remove_thread_at: eliminar un hilo de la cola de listos en una posición específica.
 * es diferente a dequeue_thread, ya que esta función elimina un hilo en una posición específica.
 * 
 * @param position 
 */
void remove_thread_at(int position)
{
    if (position < 0 || position >= queue->count)
    {
        return;
    }

    ReadyQueueNode *temp = queue->head;
    ReadyQueueNode *prev = NULL;

    if (position == 0)
    {
        queue->head = temp->next;
        free(temp);
    }
    else
    {
        for (int i = 0; temp != NULL && i < position; i++)
        {
            prev = temp;
            temp = temp->next;
        }

        if (temp == NULL)
        {
            return;
        }

        prev->next = temp->next;
        free(temp);
    }

    queue->count--;
}
/**
 * @brief update_ready_queue: actualiza la cola de listos según el tipo de calendarización.
 * 
 */
void update_ready_queue()
{
    switch (scheduling_type)
    {
    case 0:
        sort_by_priority();
        break;
    case 1:
        sort_by_sjf();
        break;
    case 2:
        sort_by_fcfs();
        break;
    default:
        break;
    }
}
/**
 * @brief print_ready_queue: imprime la cola de listos.
 * 
 */
void print_ready_queue()
{
    if (queue->head == NULL)
    {
        printf("La cola de listos está vacía.\n");
        return;
    }

    ReadyQueueNode *temp = queue->head;
    printf("Cola de listos:\n");
    while (temp != NULL)
    {
        printf("Hilo ID: %d, Prioridad: %d, Tiempo de ráfaga: %d, Tiempo de llegada: %d\n",
               temp->thread.thread_id, temp->thread.priority, temp->thread.burst_time, temp->thread.arrival_time);
        temp = temp->next;
    }
    printf("\n");
}

/**
 * @brief sort_by_priority: ordena la cola de listos por prioridad.La prioridad más baja se coloca al principio de la cola.
 * el número más bajo es la prioridad más alta.
 * 
 */
void sort_by_priority()
{
    if (queue->count < 2)
    {
        return;
    }

    for (ReadyQueueNode *i = queue->head; i != NULL; i = i->next)
    {
        for (ReadyQueueNode *j = i->next; j != NULL; j = j->next)
        {
            if (i->thread.priority > j->thread.priority)
            {
                CEthread temp = i->thread;
                i->thread = j->thread;
                j->thread = temp;
            }
        }
    }
}
/**
 * @brief sort_by_sjf: ordena la cola de listos por el tiempo de ráfaga.
 * Es decir el burst time. El hilo con el tiempo de ráfaga más corto se coloca al principio de la cola.
 * 
 */
void sort_by_sjf()
{
    if (queue->count < 2)
    {
        return;
    }

    for (ReadyQueueNode *i = queue->head; i != NULL; i = i->next)
    {
        for (ReadyQueueNode *j = i->next; j != NULL; j = j->next)
        {
            if (i->thread.burst_time > j->thread.burst_time)
            {
                CEthread temp = i->thread;
                i->thread = j->thread;
                j->thread = temp;
            }
        }
    }
}
/** 
 * @brief sort_by_fcfs: ordena la cola de listos por el tiempo de llegada.
 */
void sort_by_fcfs()
{
    if (queue->count < 2)
    {
        return;
    }

    for (ReadyQueueNode *i = queue->head; i != NULL; i = i->next)
    {
        for (ReadyQueueNode *j = i->next; j != NULL; j = j->next)
        {
            if (i->thread.arrival_time > j->thread.arrival_time)
            {
                CEthread temp = i->thread;
                i->thread = j->thread;
                j->thread = temp;
            }
        }
    }
}
/**
 * @brief calendar: función de calendarización que selecciona el algoritmo de calendarización a utilizar.
 * 
 */
void calendar()
{
    switch (scheduling_type)
    {
    case 0:
        printf("Usando Priority Scheduling\n");
        sort_by_priority();
        break;
    case 1:
        printf("Usando Shortest Job First (SJF)\n");
        sort_by_sjf();
        break;
    case 2:
        printf("Usando First Come First Served (FCFS)\n");
        sort_by_fcfs();
        break;
    default:
        printf("Tipo de calendarización no válido\n");
        break;
    }
}
