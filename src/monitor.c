/*
 * monitor.c
 * Created on: 10/06/2021
 *
 * author: @KevinRodriguez
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include "ready_queue.h"

// Declaración de la variable global
extern ReadyQueue *queue;
/**
 * @brief main: Este archivo es el monitor que se encarga de mostrar la cola de listos.
 * Lo que hace es abrir la memoria compartida y mostrar la cola de listos. Luego de eso,
 * se queda en un ciclo infinito mostrando la cola de listos cada segundo.
 *
 * @return int
 */
int main()
{
    int shm_fd = shm_open("/ready_queue_shm", O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("Error al abrir la memoria compartida");
        return 1;
    }

    queue = mmap(NULL, sizeof(ReadyQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (queue == MAP_FAILED)
    {
        perror("Error al mapear la memoria compartida");
        close(shm_fd);
        return 1;
    }

    if (queue == NULL || queue->count < 0)
    {
        fprintf(stderr, "Error: la cola de listos no se ha inicializado correctamente\n");
        munmap(queue, sizeof(ReadyQueue));
        close(shm_fd);
        return 1;
    }

    while (1)
    {
        printf("\033[H\033[J");

        print_ready_queue(queue);

        sleep(1);
    }

    munmap(queue, sizeof(ReadyQueue));
    close(shm_fd);

    return 0;
}