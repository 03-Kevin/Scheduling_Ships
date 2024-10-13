/*
 * main.c
 *
 *  Created on: 10/06/2021
 *  author: @KevinRodriguez
 *
 */

#include <stdio.h>
#include <unistd.h>
#include "ready_queue.h"

// Declaración de la variable global
extern ReadyQueue *queue;
   
/**
 * @brief main: Función principal del monitor
 *  inicia la memoria compartida y muestra la cola de listos.
 */
int main()
{
    init_ready_queue();
    add_thread(queue, 1, 1, 10, 0);
    add_thread(queue, 2, 2, 5, 1);
    add_thread(queue, 3, 1, 8, 2);

    int id = 4;
    while (1)
    {
        if (id % 3 == 0)
        {
            add_thread(queue, id, 1, 6, id);
        }
        id++;
        sleep(1);
    }

    return 0;
}