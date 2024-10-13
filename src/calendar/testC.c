#include "calendarizador.h"

int main()
{
    create_ready_queue();

    // Crear hilos de prueba
    CEthread thread1 = {1, READY, 0, 3, 10, 20, 1, NULL, NULL};
    CEthread thread2 = {2, READY, 0, 1, 10, 10, 2, NULL, NULL};
    CEthread thread3 = {3, READY, 0, 2, 10, 5, 3, NULL, NULL};

    enqueue_thread(thread1);
    enqueue_thread(thread2);
    enqueue_thread(thread3);

    // Mostrar la cola inicial
    printf("Cola inicial:\n");
    print_ready_queue();

    // Calendarizar por prioridad
    scheduling_type = 0;
    printf("Calendarizando por prioridad:\n");
    calendar();
    print_ready_queue();  // Mostrar cola tras calendarizar por prioridad
/*
    // Calendarizar por SJF
    scheduling_type = 1;
    printf("Calendarizando por SJF:\n");
    //calendar();
    print_ready_queue();  // Mostrar cola tras calendarizar por SJF

    // Calendarizar por FCFS
    scheduling_type = 2;
    printf("Calendarizando por FCFS:\n");
    calendar();
    print_ready_queue();  // Mostrar cola tras calendarizar por FCFS
*/
    // Agregar 10 hilos adicionales con la misma prioridad y mostrar la cola después de cada ingreso
    for (int i = 4; i <= 13; i++) {
        CEthread new_thread = {i, READY, 0, 1, 10, 5 + i, i, NULL, NULL};
        enqueue_thread(new_thread);
        printf("Cola después de agregar el hilo %d:\n", i);
        print_ready_queue();
    }

    return 0;
}