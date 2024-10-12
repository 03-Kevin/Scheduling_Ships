/*
 * main2.c
 *
 *  Created on: 10/08/2021
 *  author: @KevinRodriguez
 *
 */
#include <stdio.h>
#include "ready_queue.h"
#include "algorithms.h"
/**
 * @brief main2.c Función para llamar a las funciones de ordenamiento
 * 
 * @return int 
 */
int main()
{
    init_ready_queue();

    add_thread(queue, 1, 2, 8, 0);  
    add_thread(queue, 2, 1, 4, 1);
    add_thread(queue, 3, 3, 2, 2);
    add_thread(queue, 4, 2, 5, 3);

    printf("Cola de listos original:\n");
    print_ready_queue(queue);

    printf("\nOrdenando por Prioridad:\n");
    sort_by_priority(queue);
    print_ready_queue(queue);

    printf("\nOrdenando por SJF:\n");
    sort_by_sjf(queue);
    print_ready_queue(queue);

    printf("\nOrdenando por FCFS:\n");
    sort_by_fcfs(queue);
    print_ready_queue(queue);

    return 0;
}
