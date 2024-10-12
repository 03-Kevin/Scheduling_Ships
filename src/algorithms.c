/**
 * @file algorithms.c
 * @author Kevin (kjrodriguez@estudantec.cr)
 * @brief 
 * @version 0.1
 * @date 2024-10-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include "ready_queue.h"
#include "algorithms.h"
/**
 * @brief sort_by_priority: esta función se encarga de ordenar la cola de listos por prioridad.
 * 
 * @param queue refleja la cola de listos
 */
void sort_by_priority(ReadyQueue *queue)
{
    lock(&queue->lock);
    for (int i = 0; i < queue->count - 1; i++)
    {
        for (int j = 0; j < queue->count - i - 1; j++)
        {
            if (queue->threads[j].priority > queue->threads[j + 1].priority)
            {
                Thread temp = queue->threads[j];
                queue->threads[j] = queue->threads[j + 1];
                queue->threads[j + 1] = temp;
            }
        }
    }
    unlock(&queue->lock);
}
/**
 * @brief sort_by_sjf: esta función se encarga de ordenar la cola de listos por el tiempo de ráfaga.
 * 
 * @param queue refleja la cola de listos
 */
void sort_by_sjf(ReadyQueue *queue)
{
    lock(&queue->lock);
    for (int i = 0; i < queue->count - 1; i++)
    {
        for (int j = 0; j < queue->count - i - 1; j++)
        {
            if (queue->threads[j].burst_time > queue->threads[j + 1].burst_time)
            {
                Thread temp = queue->threads[j];
                queue->threads[j] = queue->threads[j + 1];
                queue->threads[j + 1] = temp;
            }
        }
    }
    unlock(&queue->lock);
}
/**
 * @brief sort_by_fcfs: esta función se encarga de ordenar la cola de listos por el tiempo de llegada.
 * 
 * @param queue refleja la cola de listos
 */
void sort_by_fcfs(ReadyQueue *queue)
{
    lock(&queue->lock);
    for (int i = 0; i < queue->count - 1; i++)
    {
        for (int j = 0; j < queue->count - i - 1; j++)
        {
            if (queue->threads[j].arrival_time > queue->threads[j + 1].arrival_time)
            {
                Thread temp = queue->threads[j];
                queue->threads[j] = queue->threads[j + 1];
                queue->threads[j + 1] = temp;
            }
        }
    }
    unlock(&queue->lock);
}
