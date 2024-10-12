/*
 * ready_queue.c
 * Created on: 10/06/2021
 * author: @KevinRodriguez
 */
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include "ready_queue.h"

// Variable global para la cola de listos en memoria compartida
ReadyQueue *queue;

/**
 * @brief init_ready_queue: Inicializa la cola de listos en memoria compartida.
 *
 */
void init_ready_queue()
{
    int shm_fd = shm_open("/ready_queue_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("Error al crear la memoria compartida");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(ReadyQueue)) == -1)
    {
        perror("Error al ajustar el tamaño de la memoria compartida");
        exit(1);
    }

    queue = mmap(NULL, sizeof(ReadyQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (queue == MAP_FAILED)
    {
        perror("Error al mapear la memoria compartida");
        exit(1);
    }

    queue->count = 0;
    queue->lock = 0;
}

/**
 * @brief add_thread: Agrega un hilo a la cola de listos.
 *          esta función se encarga de agregar un hilo a la cola de listos.
 *
 * @param queue: Cola de listos
 * @param id: ID del hilo
 * @param priority: Prioridad del hilo
 * @param burst_time: Tiempo de ráfaga
 * @param arrival_time: Tiempo de llegada
 */
void add_thread(ReadyQueue *queue, int id, int priority, int burst_time, int arrival_time)
{
    lock(&queue->lock);
    if (queue->count < MAX_THREADS)
    {
        Thread new_thread;
        new_thread.id = id;
        new_thread.priority = priority;
        new_thread.burst_time = burst_time;
        new_thread.arrival_time = arrival_time;
        queue->threads[queue->count++] = new_thread;
    }
    else
    {
        printf("Error: La cola de listos está llena.\n");
    }
    unlock(&queue->lock);
}
void print_ready_queue(ReadyQueue *queue)
{
    lock(&queue->lock);
    printf("Cola de listos:\n");
    for (int i = 0; i < queue->count; i++)
    {
        printf("Hilo ID: %d, Prioridad: %d, Tiempo de ráfaga: %d, Tiempo de llegada: %d\n",
               queue->threads[i].id, queue->threads[i].priority, queue->threads[i].burst_time, queue->threads[i].arrival_time);
    }
    unlock(&queue->lock);
}

/**
 * @brief remove_thread: Elimina un hilo de la cola de listos.
 *
 * @param queue: Cola de listos
 * @return Thread
 */
Thread remove_thread(ReadyQueue *queue)
{
    lock(&queue->lock);

    if (queue->count > 0)
    {
        Thread first_thread = queue->threads[0];

        for (int i = 1; i < queue->count; i++)
        {
            queue->threads[i - 1] = queue->threads[i];
        }

        queue->count--;

        unlock(&queue->lock);
        return first_thread;
    }
    else
    {
        printf("Error: La cola de listos está vacía.\n");
        unlock(&queue->lock);
        exit(EXIT_FAILURE);
    }
}