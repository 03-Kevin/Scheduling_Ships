#ifndef CE_THREAD_H
#define CE_THREAD_H

#include <stdio.h>

#define LISTO 0
#define EJECUTANDO 1
#define TERMINADO 2

// Estructura para representar un hilo (barco)
typedef struct
{
    int thread_id;
    int estado;                    // Estado del hilo (listo, ejecutando, terminado)
    void (*start_routine)(void *); // Función que ejecutará el hilo
    void *arg;                     // Argumentos para la función del hilo
} CEthread;

// Prototipos de funciones de CEthreads

// Crea un nuevo hilo (barco) y lo pone en la cola de listos
int CEthread_create(CEthread *thread, void (*start_routine)(void *), void *arg);

// Finaliza el hilo (barco) una vez que ha completado su tarea
void CEthread_end(CEthread *thread);

// Espera a que un hilo (barco) termine
void CEthread_join(CEthread *thread);

#endif
