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

ReadyQueue *queue = NULL; // Global queue
int scheduling_type = 0;  // Global scheduling type

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
 * @brief enqueue_thread: add a thread to the ready queue and update the queue automatically.
 *
 * @param thread
 */
void enqueue_thread(CEthread *thread) // Use a pointer to CEthread
{

    ReadyQueueNode *new_node = (ReadyQueueNode *)malloc(sizeof(ReadyQueueNode));
    new_node->thread = thread; // Store the pointer
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

    // Update the queue automatically
    update_ready_queue();
}

/**
 * @brief dequeue_thread: extract a thread from the ready queue. Always extracts the first thread in the queue.
 * It removes it from the ready queue.
 *
 * @return CEthread*
 */
CEthread *dequeue_thread() // Return a pointer to CEthread
{
    if (queue->head == NULL)
    {
        return NULL; // Queue is empty
    }

    ReadyQueueNode *temp = queue->head;
    CEthread *thread = temp->thread; // Get the pointer to the thread
    queue->head = queue->head->next;
    queue->count--;
    free(temp);

    return thread; // Return the pointer to the thread
}

/**
 * @brief remove_thread_at: remove a thread from the ready queue at a specific position.
 * This is different from dequeue_thread, as this function removes a thread at a specific position.
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
 * @brief update_ready_queue: updates the ready queue according to the scheduling type.
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
    case 3:
        sort_by_fcfs();
        break;
    case 4:
        sort_by_sjf();
        break;
    default:
        break;
    }
}

/**
 * @brief print_ready_queue: prints the ready queue.
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
               temp->thread->thread_id, temp->thread->priority, temp->thread->burst_time, temp->thread->arrival_time); // Use pointer dereference
        temp = temp->next;
    }
    printf("\n");
}

/**
 * @brief sort_by_priority: sorts the ready queue by priority. The lowest priority is placed at the front of the queue.
 * The lower number is the highest priority.
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
            if (i->thread->priority > j->thread->priority ||
                (i->thread->priority == j->thread->priority && i->thread->arrival_time > j->thread->arrival_time)) // Compare arrival_time if priorities are equal
            {
                CEthread *temp = i->thread; // Use pointers
                i->thread = j->thread;
                j->thread = temp;
            }
        }
    }
}

/**
 * @brief sort_by_sjf: sorts the ready queue by burst time.
 * That is, the burst time. The thread with the shortest burst time is placed at the front of the queue.
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
            if (i->thread->burst_time > j->thread->burst_time ||
                (i->thread->burst_time == j->thread->burst_time && i->thread->arrival_time > j->thread->arrival_time)) // Compare arrival_time if burst_time is equal
            {
                CEthread *temp = i->thread; // Use pointers
                i->thread = j->thread;
                j->thread = temp;
            }
        }
    }
}

/**
 * @brief sort_by_fcfs: sorts the ready queue by arrival time.
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
            if (i->thread->arrival_time > j->thread->arrival_time) // Use pointer dereference
            {
                CEthread *temp = i->thread; // Use pointers
                i->thread = j->thread;
                j->thread = temp;
            }
        }
    }
}

/**
 * @brief calendar: scheduling function that selects the scheduling algorithm to use.
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
    case 3:
        printf("Usando Round Robin(RR)\n");
        sort_by_fcfs();
        break;
    case 4:
        printf("Usando Earliest Deadline First (EDF)\n");
        sort_by_sjf();
        break;
    default:
        printf("Tipo de calendarización no válido\n");
        break;
    }
}
